import uuid

import pytest


FULL_VACANCY = {
    'title': 'C++ backend intern',
    'description': 'Write services on userver',
    'requirements': ['C++20', 'PostgreSQL'],
    'salary_range': '100k-150k',
    'location': 'Moscow',
    'work_mode': 'hybrid',
    'experience_level': 'junior',
}


# ---------------------------------------------------------------- create


async def test_create_vacancy(service_client, recruiter):
    response = await service_client.post('/vacancies', json=FULL_VACANCY, headers=recruiter.headers)
    assert response.status == 201
    body = response.json()
    for key, value in FULL_VACANCY.items():
        assert body[key] == value
    assert body['recruiter_id'] == recruiter.id
    assert body['is_active'] is True
    assert body['id']
    assert body['created_at']


async def test_create_vacancy_minimal(service_client, recruiter):
    response = await service_client.post('/vacancies', json={'title': 'Minimal'}, headers=recruiter.headers)
    assert response.status == 201
    body = response.json()
    assert body['description'] is None
    assert body['requirements'] is None


async def test_intern_cannot_create_vacancy(service_client, intern):
    response = await service_client.post('/vacancies', json={'title': 'Nope'}, headers=intern.headers)
    assert response.status == 403


async def test_create_vacancy_requires_auth(service_client):
    response = await service_client.post('/vacancies', json={'title': 'Nope'})
    assert response.status == 401


async def test_create_vacancy_duplicate_title(service_client, recruiter, create_vacancy):
    await create_vacancy(recruiter, title='Same')
    response = await service_client.post('/vacancies', json={'title': 'Same'}, headers=recruiter.headers)
    assert response.status == 409


# ---------------------------------------------------------------- public listing


async def test_list_vacancies_is_public(service_client, recruiter, create_vacancy):
    await create_vacancy(recruiter, title='Public one')
    response = await service_client.get('/vacancies')
    assert response.status == 200
    assert [v['title'] for v in response.json()] == ['Public one']


async def test_list_vacancies_newest_first_with_pagination(service_client, recruiter, create_vacancy):
    for i in range(5):
        await create_vacancy(recruiter, title=f'Vacancy {i}')

    page = await service_client.get('/vacancies', params={'limit': 2, 'offset': 1})
    assert page.status == 200
    assert [v['title'] for v in page.json()] == ['Vacancy 3', 'Vacancy 2']


async def test_list_vacancies_default_limit(service_client, recruiter, create_vacancy):
    for i in range(17):
        await create_vacancy(recruiter, title=f'Vacancy {i}')
    response = await service_client.get('/vacancies')
    assert len(response.json()) == 15


async def test_list_vacancies_full_text_search(service_client, recruiter, create_vacancy):
    await create_vacancy(recruiter, title='Python developer', description='Django')
    await create_vacancy(recruiter, title='C++ developer', description='userver framework')
    # NOTE: testsuite runs initdb with --lc-ctype=C, so Cyrillic is not case-folded there;
    # keep the title lowercase to test russian stemming only
    await create_vacancy(recruiter, title='аналитик данных', description='SQL')

    english = await service_client.get('/vacancies', params={'query': 'userver'})
    assert [v['title'] for v in english.json()] == ['C++ developer']

    russian = await service_client.get('/vacancies', params={'query': 'аналитика'})
    assert [v['title'] for v in russian.json()] == ['аналитик данных']


async def test_list_hides_inactive_vacancies(service_client, recruiter, create_vacancy):
    vacancy = await create_vacancy(recruiter, title='Hidden')
    await create_vacancy(recruiter, title='Visible')
    await service_client.patch(f'/vacancies/{vacancy["id"]}/toggle', headers=recruiter.headers)

    response = await service_client.get('/vacancies')
    assert [v['title'] for v in response.json()] == ['Visible']


@pytest.mark.parametrize(
    'params',
    [
        {'limit': 'abc'},
        {'offset': 'abc'},
        {'limit': '-1'},
        {'offset': '-5'},
        {'limit': '0'},
        {'limit': '101'},
        {'limit': '10abc'},
    ],
)
async def test_list_vacancies_invalid_pagination(service_client, params):
    response = await service_client.get('/vacancies', params=params)
    assert response.status == 400


@pytest.mark.parametrize(
    'payload',
    [
        {'title': ''},
        {'title': 't' * 201},
        {'title': 'ok', 'work_mode': 'sometimes'},
        {'title': 'ok', 'experience_level': 'guru'},
    ],
)
async def test_create_vacancy_validation(service_client, recruiter, payload):
    response = await service_client.post('/vacancies', json=payload, headers=recruiter.headers)
    assert response.status == 400


async def test_update_vacancy_validation(service_client, recruiter, create_vacancy):
    vacancy = await create_vacancy(recruiter)
    for payload in [{'title': ''}, {'work_mode': 'sometimes'}, {'experience_level': 'guru'}]:
        response = await service_client.patch(f'/vacancies/{vacancy["id"]}', json=payload, headers=recruiter.headers)
        assert response.status == 400


async def test_update_unknown_vacancy(service_client, recruiter):
    response = await service_client.patch(f'/vacancies/{uuid.uuid4()}', json={'title': 'x'}, headers=recruiter.headers)
    assert response.status == 404


# ---------------------------------------------------------------- get by id / me


async def test_get_vacancy_by_id_is_public(service_client, recruiter, create_vacancy):
    vacancy = await create_vacancy(recruiter, **FULL_VACANCY)
    response = await service_client.get(f'/vacancies/{vacancy["id"]}')
    assert response.status == 200
    assert response.json() == vacancy


async def test_get_unknown_vacancy(service_client):
    response = await service_client.get(f'/vacancies/{uuid.uuid4()}')
    assert response.status == 404


async def test_get_vacancy_by_invalid_id(service_client):
    response = await service_client.get('/vacancies/not-a-uuid')
    assert response.status == 400


async def test_get_my_vacancies(service_client, register, create_vacancy):
    alice = await register('recruiter')
    bob = await register('recruiter')
    await create_vacancy(alice, title='A1')
    await create_vacancy(alice, title='A2')
    await create_vacancy(bob, title='B1')

    response = await service_client.get('/vacancies/me', headers=alice.headers)
    assert response.status == 200
    assert sorted(v['title'] for v in response.json()) == ['A1', 'A2']


async def test_intern_cannot_get_my_vacancies(service_client, intern):
    response = await service_client.get('/vacancies/me', headers=intern.headers)
    assert response.status == 403


# ---------------------------------------------------------------- update


async def test_update_vacancy(service_client, recruiter, create_vacancy):
    vacancy = await create_vacancy(recruiter, **FULL_VACANCY)
    response = await service_client.patch(
        f'/vacancies/{vacancy["id"]}',
        json={'title': 'Renamed', 'requirements': ['Go'], 'location': None},
        headers=recruiter.headers,
    )
    assert response.status == 200
    body = response.json()
    assert body['title'] == 'Renamed'
    assert body['requirements'] == ['Go']
    assert body['location'] is None
    assert body['description'] == FULL_VACANCY['description']

    fetched = (await service_client.get(f'/vacancies/{vacancy["id"]}')).json()
    assert fetched['title'] == 'Renamed'
    assert fetched['location'] is None


async def test_update_vacancy_returns_fresh_updated_at(service_client, recruiter, create_vacancy):
    vacancy = await create_vacancy(recruiter)
    response = await service_client.patch(
        f'/vacancies/{vacancy["id"]}', json={'title': 'Renamed'}, headers=recruiter.headers
    )
    fetched = (await service_client.get(f'/vacancies/{vacancy["id"]}')).json()

    assert fetched['updated_at'] != vacancy['updated_at']
    assert response.json()['updated_at'] == fetched['updated_at']


async def test_update_vacancy_empty_body(service_client, recruiter, create_vacancy):
    vacancy = await create_vacancy(recruiter)
    response = await service_client.patch(f'/vacancies/{vacancy["id"]}', json={}, headers=recruiter.headers)
    assert response.status == 400


async def test_update_vacancy_nothing_changed(service_client, recruiter, create_vacancy):
    vacancy = await create_vacancy(recruiter, title='Same')
    response = await service_client.patch(
        f'/vacancies/{vacancy["id"]}', json={'title': 'Same'}, headers=recruiter.headers
    )
    assert response.status == 400


async def test_update_vacancy_of_other_recruiter(service_client, register, create_vacancy):
    alice = await register('recruiter')
    bob = await register('recruiter')
    vacancy = await create_vacancy(bob, title='Bob vacancy')
    response = await service_client.patch(
        f'/vacancies/{vacancy["id"]}', json={'title': 'Hacked'}, headers=alice.headers
    )
    assert response.status == 403

    fetched = (await service_client.get(f'/vacancies/{vacancy["id"]}')).json()
    assert fetched['title'] == 'Bob vacancy'


async def test_update_vacancy_duplicate_title(service_client, recruiter, create_vacancy):
    await create_vacancy(recruiter, title='Taken')
    vacancy = await create_vacancy(recruiter, title='Other')
    response = await service_client.patch(
        f'/vacancies/{vacancy["id"]}', json={'title': 'Taken'}, headers=recruiter.headers
    )
    assert response.status == 409


async def test_intern_cannot_update_vacancy(service_client, recruiter, intern, create_vacancy):
    vacancy = await create_vacancy(recruiter)
    response = await service_client.patch(
        f'/vacancies/{vacancy["id"]}', json={'title': 'Hacked'}, headers=intern.headers
    )
    assert response.status == 403


# ---------------------------------------------------------------- toggle


async def test_toggle_vacancy(service_client, recruiter, create_vacancy):
    vacancy = await create_vacancy(recruiter)

    off = await service_client.patch(f'/vacancies/{vacancy["id"]}/toggle', headers=recruiter.headers)
    assert off.status == 200
    assert off.json()['is_active'] is False

    on = await service_client.patch(f'/vacancies/{vacancy["id"]}/toggle', headers=recruiter.headers)
    assert on.status == 200
    assert on.json()['is_active'] is True


async def test_toggle_vacancy_of_other_recruiter(service_client, register, create_vacancy):
    alice = await register('recruiter')
    vacancy = await create_vacancy(await register('recruiter'))
    response = await service_client.patch(f'/vacancies/{vacancy["id"]}/toggle', headers=alice.headers)
    assert response.status == 403

    fetched = (await service_client.get(f'/vacancies/{vacancy["id"]}')).json()
    assert fetched['is_active'] is True


async def test_intern_cannot_toggle_vacancy(service_client, recruiter, intern, create_vacancy):
    vacancy = await create_vacancy(recruiter)
    response = await service_client.patch(f'/vacancies/{vacancy["id"]}/toggle', headers=intern.headers)
    assert response.status == 403


# ---------------------------------------------------------------- delete


async def test_delete_vacancy(service_client, recruiter, create_vacancy):
    vacancy = await create_vacancy(recruiter)
    response = await service_client.delete(f'/vacancies/{vacancy["id"]}', headers=recruiter.headers)
    assert response.status == 204

    fetched = await service_client.get(f'/vacancies/{vacancy["id"]}')
    assert fetched.status == 404


async def test_delete_vacancy_of_other_recruiter(service_client, register, create_vacancy):
    alice = await register('recruiter')
    vacancy = await create_vacancy(await register('recruiter'))
    response = await service_client.delete(f'/vacancies/{vacancy["id"]}', headers=alice.headers)
    assert response.status == 403

    fetched = await service_client.get(f'/vacancies/{vacancy["id"]}')
    assert fetched.status == 200


async def test_intern_cannot_delete_vacancy(service_client, recruiter, intern, create_vacancy):
    vacancy = await create_vacancy(recruiter)
    response = await service_client.delete(f'/vacancies/{vacancy["id"]}', headers=intern.headers)
    assert response.status == 403


async def test_delete_vacancy_removes_applications(service_client, application_setup):
    recruiter = application_setup['recruiter']
    await service_client.delete(f'/vacancies/{application_setup["vacancy"]["id"]}', headers=recruiter.headers)

    applications = await service_client.get('/applications/me', headers=application_setup['intern'].headers)
    assert applications.json() == []
