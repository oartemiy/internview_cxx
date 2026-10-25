import uuid

import aiohttp

from conftest import PDF_BYTES


def _pdf_form(content=PDF_BYTES, filename='cv.pdf'):
    form = aiohttp.FormData()
    form.add_field('file', content, filename=filename, content_type='application/pdf')
    return form


# ---------------------------------------------------------------- create / list / get


async def test_create_cv(service_client, intern):
    response = await service_client.post(
        '/cvs', json={'title': 'Backend', 'description': 'C++ developer'}, headers=intern.headers
    )
    assert response.status == 201
    body = response.json()
    assert body['title'] == 'Backend'
    assert body['description'] == 'C++ developer'
    assert body['user_id'] == intern.id
    assert body['cv_pdf'] is None
    assert body['created_at']
    assert body['updated_at']


async def test_create_cv_ignores_user_id_from_body(service_client, register):
    alice = await register('intern')
    bob = await register('intern')
    response = await service_client.post('/cvs', json={'title': 'Mine', 'user_id': bob.id}, headers=alice.headers)
    assert response.status == 201
    assert response.json()['user_id'] == alice.id


async def test_recruiter_cannot_create_cv(service_client, recruiter):
    response = await service_client.post('/cvs', json={'title': 'Backend'}, headers=recruiter.headers)
    assert response.status == 403


async def test_create_cv_duplicate_title(service_client, intern, create_cv):
    await create_cv(intern, title='Backend')
    response = await service_client.post('/cvs', json={'title': 'Backend'}, headers=intern.headers)
    assert response.status == 409


async def test_same_title_allowed_for_different_users(service_client, register, create_cv):
    await create_cv(await register('intern'), title='Backend')
    await create_cv(await register('intern'), title='Backend')


async def test_list_my_cvs(service_client, register, create_cv):
    alice = await register('intern')
    bob = await register('intern')
    await create_cv(alice, title='A1')
    await create_cv(alice, title='A2')
    await create_cv(bob, title='B1')

    response = await service_client.get('/cvs/me', headers=alice.headers)
    assert response.status == 200
    assert sorted(cv['title'] for cv in response.json()) == ['A1', 'A2']


async def test_list_my_cvs_empty(service_client, intern):
    response = await service_client.get('/cvs/me', headers=intern.headers)
    assert response.status == 200
    assert response.json() == []


async def test_recruiter_cannot_list_cvs(service_client, recruiter):
    response = await service_client.get('/cvs/me', headers=recruiter.headers)
    assert response.status == 403


async def test_get_cv_by_id(service_client, intern, create_cv):
    cv = await create_cv(intern, title='Backend')
    response = await service_client.get(f'/cvs/{cv["id"]}', headers=intern.headers)
    assert response.status == 200
    assert response.json()['id'] == cv['id']
    assert response.json()['title'] == 'Backend'


async def test_get_cv_of_other_intern(service_client, register, create_cv):
    alice = await register('intern')
    bob = await register('intern')
    cv = await create_cv(bob)
    response = await service_client.get(f'/cvs/{cv["id"]}', headers=alice.headers)
    assert response.status == 404


async def test_recruiter_gets_applicant_cv(service_client, application_setup):
    cv = application_setup['cv']
    response = await service_client.get(f'/cvs/{cv["id"]}', headers=application_setup['recruiter'].headers)
    assert response.status == 200
    assert response.json()['id'] == cv['id']


async def test_recruiter_cannot_get_non_applicant_cv(service_client, recruiter, intern, create_cv):
    cv = await create_cv(intern)
    response = await service_client.get(f'/cvs/{cv["id"]}', headers=recruiter.headers)
    assert response.status == 403


# ---------------------------------------------------------------- update


async def test_update_cv(service_client, intern, create_cv):
    cv = await create_cv(intern, title='Old', description='old')
    response = await service_client.patch(
        f'/cvs/{cv["id"]}', json={'title': 'New', 'description': None}, headers=intern.headers
    )
    assert response.status == 200
    body = response.json()
    assert body['title'] == 'New'
    assert body['description'] is None
    assert body['updated_at'] != cv['updated_at']

    fetched = (await service_client.get(f'/cvs/{cv["id"]}', headers=intern.headers)).json()
    assert fetched['title'] == 'New'
    assert fetched['description'] is None


async def test_update_cv_empty_body(service_client, intern, create_cv):
    cv = await create_cv(intern)
    response = await service_client.patch(f'/cvs/{cv["id"]}', json={}, headers=intern.headers)
    assert response.status == 400


async def test_update_cv_nothing_changed(service_client, intern, create_cv):
    cv = await create_cv(intern, title='Same')
    response = await service_client.patch(f'/cvs/{cv["id"]}', json={'title': 'Same'}, headers=intern.headers)
    assert response.status == 400


async def test_update_cv_title_length(service_client, intern, create_cv):
    cv = await create_cv(intern)
    for title in ['', 't' * 201]:
        response = await service_client.patch(f'/cvs/{cv["id"]}', json={'title': title}, headers=intern.headers)
        assert response.status == 400

    one_char = await service_client.patch(f'/cvs/{cv["id"]}', json={'title': 'x'}, headers=intern.headers)
    assert one_char.status == 200


async def test_create_cv_title_length(service_client, intern):
    for title in ['', 't' * 201]:
        response = await service_client.post('/cvs', json={'title': title}, headers=intern.headers)
        assert response.status == 400


async def test_update_cv_duplicate_title(service_client, intern, create_cv):
    await create_cv(intern, title='Taken')
    cv = await create_cv(intern, title='Other')
    response = await service_client.patch(f'/cvs/{cv["id"]}', json={'title': 'Taken'}, headers=intern.headers)
    assert response.status == 409


async def test_update_cv_of_other_intern(service_client, register, create_cv):
    alice = await register('intern')
    bob = await register('intern')
    cv = await create_cv(bob, title='Bob CV')
    response = await service_client.patch(f'/cvs/{cv["id"]}', json={'title': 'Hacked'}, headers=alice.headers)
    assert response.status == 404

    fetched = (await service_client.get(f'/cvs/{cv["id"]}', headers=bob.headers)).json()
    assert fetched['title'] == 'Bob CV'


# ---------------------------------------------------------------- delete


async def test_delete_cv(service_client, intern, create_cv):
    cv = await create_cv(intern)
    response = await service_client.delete(f'/cvs/{cv["id"]}', headers=intern.headers)
    assert response.status == 204

    cvs = await service_client.get('/cvs/me', headers=intern.headers)
    assert cvs.json() == []


async def test_delete_cv_of_other_intern(service_client, register, create_cv):
    alice = await register('intern')
    bob = await register('intern')
    cv = await create_cv(bob)
    response = await service_client.delete(f'/cvs/{cv["id"]}', headers=alice.headers)
    assert response.status == 404

    cvs = await service_client.get('/cvs/me', headers=bob.headers)
    assert len(cvs.json()) == 1


async def test_delete_unknown_cv(service_client, intern):
    response = await service_client.delete(f'/cvs/{uuid.uuid4()}', headers=intern.headers)
    assert response.status == 404


async def test_delete_cv_removes_applications(service_client, application_setup):
    intern = application_setup['intern']
    await service_client.delete(f'/cvs/{application_setup["cv"]["id"]}', headers=intern.headers)
    applications = await service_client.get('/applications/me', headers=intern.headers)
    assert applications.json() == []


# ---------------------------------------------------------------- pdf


async def test_upload_and_download_pdf(service_client, intern, create_cv):
    cv = await create_cv(intern)
    response = await service_client.post(f'/cvs/{cv["id"]}/pdf', data=_pdf_form(), headers=intern.headers)
    assert response.status == 200

    fetched = (await service_client.get(f'/cvs/{cv["id"]}', headers=intern.headers)).json()
    assert fetched['cv_pdf'].endswith('.pdf')

    pdf = await service_client.get(f'/cvs/{cv["id"]}/pdf', headers=intern.headers)
    assert pdf.status == 200
    assert pdf.headers['Content-Type'] == 'application/pdf'
    assert pdf.content == PDF_BYTES


async def test_reupload_pdf_replaces_old_file(service_client, intern, create_cv, uploads_dir):
    cv = await create_cv(intern)
    await service_client.post(f'/cvs/{cv["id"]}/pdf', data=_pdf_form(b'%PDF-first'), headers=intern.headers)
    first = (await service_client.get(f'/cvs/{cv["id"]}', headers=intern.headers)).json()['cv_pdf']
    await service_client.post(f'/cvs/{cv["id"]}/pdf', data=_pdf_form(b'%PDF-second'), headers=intern.headers)
    second = (await service_client.get(f'/cvs/{cv["id"]}', headers=intern.headers)).json()['cv_pdf']

    assert first != second
    assert not (uploads_dir / 'pdf' / first).exists()
    pdf = await service_client.get(f'/cvs/{cv["id"]}/pdf', headers=intern.headers)
    assert pdf.content == b'%PDF-second'


async def test_delete_cv_removes_pdf_file(service_client, intern, create_cv, uploads_dir):
    cv = await create_cv(intern)
    await service_client.post(f'/cvs/{cv["id"]}/pdf', data=_pdf_form(), headers=intern.headers)
    key = (await service_client.get(f'/cvs/{cv["id"]}', headers=intern.headers)).json()['cv_pdf']
    assert (uploads_dir / 'pdf' / key).exists()

    await service_client.delete(f'/cvs/{cv["id"]}', headers=intern.headers)
    assert not (uploads_dir / 'pdf' / key).exists()


async def test_cv_pdf_can_be_removed(service_client, intern, create_cv, uploads_dir):
    cv = await create_cv(intern)
    await service_client.post(f'/cvs/{cv["id"]}/pdf', data=_pdf_form(), headers=intern.headers)
    key = (await service_client.get(f'/cvs/{cv["id"]}', headers=intern.headers)).json()['cv_pdf']

    response = await service_client.patch(f'/cvs/{cv["id"]}', json={'cv_pdf': None}, headers=intern.headers)
    assert response.status == 200
    assert response.json()['cv_pdf'] is None
    assert not (uploads_dir / 'pdf' / key).exists()


async def test_upload_pdf_without_file_field(service_client, intern, create_cv):
    cv = await create_cv(intern)
    form = aiohttp.FormData()
    form.add_field('not_file', 'value')
    response = await service_client.post(f'/cvs/{cv["id"]}/pdf', data=form, headers=intern.headers)
    assert response.status == 400


async def test_upload_non_pdf(service_client, intern, create_cv):
    cv = await create_cv(intern)
    response = await service_client.post(
        f'/cvs/{cv["id"]}/pdf', data=_pdf_form(b'not pdf', 'cv.docx'), headers=intern.headers
    )
    assert response.status == 400


async def test_upload_pdf_to_other_intern_cv(service_client, register, create_cv):
    alice = await register('intern')
    bob = await register('intern')
    cv = await create_cv(bob)
    response = await service_client.post(f'/cvs/{cv["id"]}/pdf', data=_pdf_form(), headers=alice.headers)
    assert response.status == 404


async def test_recruiter_downloads_applicant_pdf(service_client, application_setup):
    intern = application_setup['intern']
    cv = application_setup['cv']
    await service_client.post(f'/cvs/{cv["id"]}/pdf', data=_pdf_form(), headers=intern.headers)

    pdf = await service_client.get(f'/cvs/{cv["id"]}/pdf', headers=application_setup['recruiter'].headers)
    assert pdf.status == 200
    assert pdf.content == PDF_BYTES


async def test_recruiter_cannot_download_non_applicant_pdf(service_client, recruiter, intern, create_cv):
    cv = await create_cv(intern)
    await service_client.post(f'/cvs/{cv["id"]}/pdf', data=_pdf_form(), headers=intern.headers)
    response = await service_client.get(f'/cvs/{cv["id"]}/pdf', headers=recruiter.headers)
    assert response.status == 403


async def test_download_pdf_when_absent(service_client, intern, create_cv):
    cv = await create_cv(intern)
    response = await service_client.get(f'/cvs/{cv["id"]}/pdf', headers=intern.headers)
    assert response.status == 404


async def test_cv_pdf_key_from_create_cannot_point_outside_storage(service_client, intern, uploads_dir):
    secret = uploads_dir / f'secret-{uuid.uuid4().hex}.txt'
    secret.write_text('TOP-SECRET-CONTENT')
    response = await service_client.post(
        '/cvs', json={'title': 'Evil', 'cv_pdf': f'../{secret.name}'}, headers=intern.headers
    )
    if response.status != 200:
        return  # rejecting such a request is a valid fix
    pdf = await service_client.get(f'/cvs/{response.json()["id"]}/pdf', headers=intern.headers)
    assert 'TOP-SECRET-CONTENT' not in pdf.text


async def test_cv_pdf_key_from_update_cannot_point_outside_storage(service_client, intern, create_cv, uploads_dir):
    secret = uploads_dir / f'secret-{uuid.uuid4().hex}.txt'
    secret.write_text('TOP-SECRET-CONTENT')
    cv = await create_cv(intern)
    await service_client.patch(f'/cvs/{cv["id"]}', json={'cv_pdf': f'../{secret.name}'}, headers=intern.headers)

    pdf = await service_client.get(f'/cvs/{cv["id"]}/pdf', headers=intern.headers)
    assert 'TOP-SECRET-CONTENT' not in pdf.text


async def test_get_cv_by_invalid_id(service_client, intern):
    response = await service_client.get('/cvs/not-a-uuid', headers=intern.headers)
    assert response.status == 400
