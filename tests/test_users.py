import uuid

import aiohttp

from conftest import PNG_BYTES


def _file_form(content, filename, content_type='application/octet-stream'):
    form = aiohttp.FormData()
    form.add_field('file', content, filename=filename, content_type=content_type)
    return form


# ---------------------------------------------------------------- GET /users/me


async def test_get_me(service_client, register):
    user = await register('intern', login='alice', name='Alice', description='about me')
    response = await service_client.get('/users/me', headers=user.headers)
    assert response.status == 200
    body = response.json()
    assert body['id'] == user.id
    assert body['login'] == 'alice'
    assert body['name'] == 'Alice'
    assert body['role'] == 'intern'
    assert body['description'] == 'about me'
    assert 'access_token' not in body
    assert 'refresh_token' not in body
    assert 'password_hash' not in body


# ---------------------------------------------------------------- PATCH /users/me


async def test_update_me(service_client, intern):
    response = await service_client.patch(
        '/users/me', json={'name': 'New Name', 'description': 'new description'}, headers=intern.headers
    )
    assert response.status == 200
    body = response.json()
    assert body['name'] == 'New Name'
    assert body['description'] == 'new description'
    assert body['role'] == 'intern'

    me = await service_client.get('/users/me', headers=intern.headers)
    assert me.json()['name'] == 'New Name'


async def test_update_me_login_and_login_with_it(service_client, intern):
    response = await service_client.patch('/users/me', json={'login': 'renamed'}, headers=intern.headers)
    assert response.status == 200
    login = await service_client.post('/auth/login', json={'login': 'renamed', 'password': intern.password})
    assert login.status == 200


async def test_update_me_ignores_role(service_client, intern):
    response = await service_client.patch(
        '/users/me', json={'name': 'Hacker', 'role': 'recruiter'}, headers=intern.headers
    )
    assert response.status == 200
    assert response.json()['role'] == 'intern'


async def test_update_me_empty_body(service_client, intern):
    response = await service_client.patch('/users/me', json={}, headers=intern.headers)
    assert response.status == 400


async def test_update_me_nothing_changed(service_client, register):
    user = await register('intern', name='Same')
    response = await service_client.patch('/users/me', json={'name': 'Same'}, headers=user.headers)
    assert response.status == 400


async def test_update_me_reserved_login(service_client, intern):
    response = await service_client.patch('/users/me', json={'login': 'me'}, headers=intern.headers)
    assert response.status == 400


async def test_update_me_too_short_login(service_client, intern):
    response = await service_client.patch('/users/me', json={'login': 'xy'}, headers=intern.headers)
    assert response.status == 400


async def test_update_me_taken_login(service_client, register):
    await register('intern', login='taken')
    user = await register('intern')
    response = await service_client.patch('/users/me', json={'login': 'taken'}, headers=user.headers)
    assert response.status == 409


# ---------------------------------------------------------------- GET /users/{id}


async def test_get_user_by_id_self(service_client, intern):
    response = await service_client.get(f'/users/{intern.id}', headers=intern.headers)
    assert response.status == 200
    assert response.json()['id'] == intern.id
    assert 'password_hash' not in response.json()


async def test_get_user_by_id_other_intern_forbidden(service_client, register):
    alice = await register('intern')
    bob = await register('intern')
    response = await service_client.get(f'/users/{bob.id}', headers=alice.headers)
    assert response.status == 403


async def test_recruiter_gets_applicant(service_client, application_setup):
    recruiter = application_setup['recruiter']
    intern = application_setup['intern']
    response = await service_client.get(f'/users/{intern.id}', headers=recruiter.headers)
    assert response.status == 200
    assert response.json()['id'] == intern.id


async def test_recruiter_cannot_get_non_applicant(service_client, recruiter, intern):
    response = await service_client.get(f'/users/{intern.id}', headers=recruiter.headers)
    assert response.status == 403


async def test_get_user_by_invalid_id(service_client, intern):
    response = await service_client.get('/users/not-a-uuid', headers=intern.headers)
    assert response.status == 400


# ---------------------------------------------------------------- DELETE /users/me


async def test_delete_me(service_client, intern):
    response = await service_client.delete(
        '/users/me', json={'login': intern.login, 'password': intern.password}, headers=intern.headers
    )
    assert response.status == 204

    login = await service_client.post('/auth/login', json={'login': intern.login, 'password': intern.password})
    assert login.status == 401
    me = await service_client.get('/users/me', headers=intern.headers)
    assert me.status == 404


async def test_delete_me_wrong_password_keeps_user(service_client, intern):
    response = await service_client.delete(
        '/users/me', json={'login': intern.login, 'password': 'wrong-password'}, headers=intern.headers
    )
    assert response.status == 403

    me = await service_client.get('/users/me', headers=intern.headers)
    assert me.status == 200


async def test_delete_me_wrong_password_keeps_intern_cvs(service_client, intern, create_cv):
    await create_cv(intern, title='Keep me')
    response = await service_client.delete(
        '/users/me', json={'login': intern.login, 'password': 'wrong-password'}, headers=intern.headers
    )
    assert response.status == 403

    cvs = await service_client.get('/cvs/me', headers=intern.headers)
    assert [cv['title'] for cv in cvs.json()] == ['Keep me']


async def test_delete_me_wrong_password_keeps_recruiter_vacancies(service_client, recruiter, create_vacancy):
    await create_vacancy(recruiter, title='Keep me')
    response = await service_client.delete(
        '/users/me', json={'login': recruiter.login, 'password': 'wrong-password'}, headers=recruiter.headers
    )
    assert response.status == 403

    vacancies = await service_client.get('/vacancies/me', headers=recruiter.headers)
    assert [v['title'] for v in vacancies.json()] == ['Keep me']


async def test_delete_recruiter_removes_vacancies(service_client, recruiter, create_vacancy):
    vacancy = await create_vacancy(recruiter)
    response = await service_client.delete(
        '/users/me', json={'login': recruiter.login, 'password': recruiter.password}, headers=recruiter.headers
    )
    assert response.status == 204

    get = await service_client.get(f'/vacancies/{vacancy["id"]}')
    assert get.status == 404


# ---------------------------------------------------------------- profile picture


async def test_profile_pic_upload_and_download(service_client, intern):
    response = await service_client.post(
        '/users/me/profile-pic', data=_file_form(PNG_BYTES, 'avatar.png', 'image/png'), headers=intern.headers
    )
    assert response.status == 200

    me = await service_client.get('/users/me', headers=intern.headers)
    assert me.json()['profile_pic'].endswith('.png')

    pic = await service_client.get('/users/me/profile-pic', headers=intern.headers)
    assert pic.status == 200
    assert pic.headers['Content-Type'] == 'image/png'
    assert pic.content == PNG_BYTES


async def test_profile_pic_reupload_replaces_old_file(service_client, intern, uploads_dir):
    await service_client.post(
        '/users/me/profile-pic', data=_file_form(PNG_BYTES, 'first.png'), headers=intern.headers
    )
    first_key = (await service_client.get('/users/me', headers=intern.headers)).json()['profile_pic']

    await service_client.post(
        '/users/me/profile-pic', data=_file_form(b'second', 'second.jpg'), headers=intern.headers
    )
    second_key = (await service_client.get('/users/me', headers=intern.headers)).json()['profile_pic']

    assert first_key != second_key
    assert not (uploads_dir / 'img' / first_key).exists()
    assert (uploads_dir / 'img' / second_key).exists()
    pic = await service_client.get('/users/me/profile-pic', headers=intern.headers)
    assert pic.headers['Content-Type'] == 'image/jpeg'
    assert pic.content == b'second'


async def test_profile_pic_unsupported_extension(service_client, intern):
    response = await service_client.post(
        '/users/me/profile-pic', data=_file_form(b'MZ', 'virus.exe'), headers=intern.headers
    )
    assert response.status == 400


async def test_profile_pic_without_extension(service_client, intern):
    response = await service_client.post(
        '/users/me/profile-pic', data=_file_form(PNG_BYTES, 'avatar'), headers=intern.headers
    )
    assert response.status == 400


async def test_profile_pic_upload_without_file_field(service_client, intern):
    form = aiohttp.FormData()
    form.add_field('not_file', 'value')
    response = await service_client.post('/users/me/profile-pic', data=form, headers=intern.headers)
    assert response.status == 400


async def test_profile_pic_get_when_absent(service_client, intern):
    response = await service_client.get('/users/me/profile-pic', headers=intern.headers)
    assert response.status == 404


async def test_recruiter_gets_applicant_profile_pic(service_client, application_setup):
    intern = application_setup['intern']
    recruiter = application_setup['recruiter']
    await service_client.post(
        '/users/me/profile-pic', data=_file_form(PNG_BYTES, 'avatar.png'), headers=intern.headers
    )
    response = await service_client.get(f'/users/{intern.id}/profile-pic', headers=recruiter.headers)
    assert response.status == 200
    assert response.content == PNG_BYTES


async def test_recruiter_cannot_get_non_applicant_profile_pic(service_client, recruiter, intern):
    response = await service_client.get(f'/users/{intern.id}/profile-pic', headers=recruiter.headers)
    assert response.status == 403


async def test_intern_cannot_use_recruiter_profile_pic_route(service_client, register):
    alice = await register('intern')
    bob = await register('intern')
    response = await service_client.get(f'/users/{bob.id}/profile-pic', headers=alice.headers)
    assert response.status == 403


async def test_profile_pic_can_be_removed(service_client, intern, uploads_dir):
    await service_client.post(
        '/users/me/profile-pic', data=_file_form(PNG_BYTES, 'avatar.png'), headers=intern.headers
    )
    key = (await service_client.get('/users/me', headers=intern.headers)).json()['profile_pic']

    response = await service_client.patch('/users/me', json={'profile_pic': None}, headers=intern.headers)
    assert response.status == 200
    assert response.json()['profile_pic'] is None
    assert not (uploads_dir / 'img' / key).exists()


async def test_update_me_rejects_profile_pic_key(service_client, intern):
    response = await service_client.patch('/users/me', json={'profile_pic': 'any.png'}, headers=intern.headers)
    assert response.status == 400


# ---------------------------------------------------------------- path traversal


def _make_secret(uploads_dir):
    secret = uploads_dir / f'secret-{uuid.uuid4().hex}.txt'
    secret.write_text('TOP-SECRET-CONTENT')
    return secret


async def test_profile_pic_key_cannot_point_outside_storage(service_client, intern, uploads_dir):
    secret = _make_secret(uploads_dir)
    await service_client.patch('/users/me', json={'profile_pic': f'../{secret.name}'}, headers=intern.headers)

    response = await service_client.get('/users/me/profile-pic', headers=intern.headers)
    assert 'TOP-SECRET-CONTENT' not in response.text


async def test_profile_pic_key_cannot_delete_files_outside_storage(service_client, intern, uploads_dir):
    secret = _make_secret(uploads_dir)
    await service_client.patch('/users/me', json={'profile_pic': f'../{secret.name}'}, headers=intern.headers)
    await service_client.patch('/users/me', json={'profile_pic': None}, headers=intern.headers)

    assert secret.exists()


async def test_register_profile_pic_key_cannot_point_outside_storage(service_client, register, uploads_dir):
    secret = _make_secret(uploads_dir)
    response = await service_client.post(
        '/auth/register',
        json={
            'login': 'mallory',
            'password': 'password123',
            'name': 'Mallory',
            'role': 'intern',
            'profile_pic': f'../{secret.name}',
        },
    )
    if response.status != 200:
        return  # rejecting such a request is a valid fix
    token = response.json()['access_token']
    pic = await service_client.get('/users/me/profile-pic', headers={'Authorization': f'Bearer {token}'})
    assert 'TOP-SECRET-CONTENT' not in pic.text
