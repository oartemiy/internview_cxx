import uuid


# ---------------------------------------------------------------- create


async def test_apply(service_client, intern, recruiter, create_cv, create_vacancy):
    cv = await create_cv(intern)
    vacancy = await create_vacancy(recruiter)
    response = await service_client.post(
        '/applications',
        json={'vacancy_id': vacancy['id'], 'cv_id': cv['id'], 'cover_letter': 'Hire me'},
        headers=intern.headers,
    )
    assert response.status == 201
    body = response.json()
    assert body['vacancy_id'] == vacancy['id']
    assert body['cv_id'] == cv['id']
    assert body['intern_id'] == intern.id
    assert body['cover_letter'] == 'Hire me'
    assert body['status'] == 'pending'


async def test_apply_ignores_intern_id_from_body(service_client, register, recruiter, create_cv, create_vacancy):
    alice = await register('intern')
    bob = await register('intern')
    cv = await create_cv(alice)
    vacancy = await create_vacancy(recruiter)
    response = await service_client.post(
        '/applications',
        json={'vacancy_id': vacancy['id'], 'cv_id': cv['id'], 'intern_id': bob.id},
        headers=alice.headers,
    )
    assert response.status == 201
    assert response.json()['intern_id'] == alice.id


async def test_apply_twice(service_client, application_setup):
    response = await service_client.post(
        '/applications',
        json={'vacancy_id': application_setup['vacancy']['id'], 'cv_id': application_setup['cv']['id']},
        headers=application_setup['intern'].headers,
    )
    assert response.status == 409


async def test_apply_with_other_intern_cv(service_client, register, recruiter, create_cv, create_vacancy):
    alice = await register('intern')
    bob_cv = await create_cv(await register('intern'))
    vacancy = await create_vacancy(recruiter)
    response = await service_client.post(
        '/applications', json={'vacancy_id': vacancy['id'], 'cv_id': bob_cv['id']}, headers=alice.headers
    )
    assert response.status == 404


async def test_apply_to_unknown_vacancy(service_client, intern, create_cv):
    cv = await create_cv(intern)
    response = await service_client.post(
        '/applications', json={'vacancy_id': str(uuid.uuid4()), 'cv_id': cv['id']}, headers=intern.headers
    )
    assert response.status == 404


async def test_recruiter_cannot_apply(service_client, recruiter, create_vacancy):
    vacancy = await create_vacancy(recruiter)
    response = await service_client.post(
        '/applications', json={'vacancy_id': vacancy['id'], 'cv_id': str(uuid.uuid4())}, headers=recruiter.headers
    )
    assert response.status == 403


# ---------------------------------------------------------------- list


async def test_intern_lists_own_applications(service_client, application_setup, register):
    other = await register('intern')
    intern_apps = await service_client.get('/applications/me', headers=application_setup['intern'].headers)
    assert [a['id'] for a in intern_apps.json()] == [application_setup['application']['id']]

    other_apps = await service_client.get('/applications/me', headers=other.headers)
    assert other_apps.json() == []


async def test_recruiter_lists_applications_to_own_vacancies(service_client, application_setup, register):
    recruiter_apps = await service_client.get('/applications/me', headers=application_setup['recruiter'].headers)
    assert [a['id'] for a in recruiter_apps.json()] == [application_setup['application']['id']]

    other_recruiter = await register('recruiter')
    other_apps = await service_client.get('/applications/me', headers=other_recruiter.headers)
    assert other_apps.json() == []


async def test_vacancy_applications_for_owner(service_client, application_setup):
    vacancy_id = application_setup['vacancy']['id']
    response = await service_client.get(
        f'/vacancies/{vacancy_id}/applications', headers=application_setup['recruiter'].headers
    )
    assert response.status == 200
    assert [a['id'] for a in response.json()] == [application_setup['application']['id']]


async def test_vacancy_applications_for_other_recruiter(service_client, application_setup, register):
    other = await register('recruiter')
    vacancy_id = application_setup['vacancy']['id']
    response = await service_client.get(f'/vacancies/{vacancy_id}/applications', headers=other.headers)
    assert response.status == 403


async def test_vacancy_applications_for_intern(service_client, application_setup):
    vacancy_id = application_setup['vacancy']['id']
    response = await service_client.get(
        f'/vacancies/{vacancy_id}/applications', headers=application_setup['intern'].headers
    )
    assert response.status == 403


# ---------------------------------------------------------------- recruiter updates status


async def test_recruiter_updates_status(service_client, application_setup):
    application = application_setup['application']
    response = await service_client.patch(
        f'/applications/{application["id"]}', json={'status': 'approved'},
        headers=application_setup['recruiter'].headers,
    )
    assert response.status == 200
    assert response.json()['status'] == 'approved'

    intern_view = await service_client.get('/applications/me', headers=application_setup['intern'].headers)
    assert intern_view.json()[0]['status'] == 'approved'


async def test_recruiter_sets_unsupported_status(service_client, application_setup):
    application = application_setup['application']
    response = await service_client.patch(
        f'/applications/{application["id"]}', json={'status': 'hired-immediately'},
        headers=application_setup['recruiter'].headers,
    )
    assert response.status == 400


async def test_recruiter_cannot_change_cover_letter(service_client, application_setup):
    application = application_setup['application']
    response = await service_client.patch(
        f'/applications/{application["id"]}', json={'cover_letter': 'edited by recruiter'},
        headers=application_setup['recruiter'].headers,
    )
    assert response.status == 400


async def test_other_recruiter_cannot_update_status(service_client, application_setup, register):
    other = await register('recruiter')
    application = application_setup['application']
    response = await service_client.patch(
        f'/applications/{application["id"]}', json={'status': 'rejected'}, headers=other.headers
    )
    assert response.status == 403


async def test_recruiter_updates_unknown_application(service_client, recruiter):
    response = await service_client.patch(
        f'/applications/{uuid.uuid4()}', json={'status': 'approved'}, headers=recruiter.headers
    )
    assert response.status == 404


async def test_update_application_refreshes_updated_at(service_client, application_setup):
    application = application_setup['application']
    response = await service_client.patch(
        f'/applications/{application["id"]}', json={'status': 'reviewed'},
        headers=application_setup['recruiter'].headers,
    )
    assert response.json()['updated_at'] != application['updated_at']


# ---------------------------------------------------------------- intern updates application


async def test_intern_updates_cover_letter(service_client, application_setup):
    application = application_setup['application']
    response = await service_client.patch(
        f'/applications/{application["id"]}', json={'cover_letter': 'Updated letter'},
        headers=application_setup['intern'].headers,
    )
    assert response.status == 200
    assert response.json()['cover_letter'] == 'Updated letter'
    assert response.json()['status'] == 'pending'


async def test_intern_switches_to_another_own_cv(service_client, application_setup, create_cv):
    intern = application_setup['intern']
    new_cv = await create_cv(intern)
    application = application_setup['application']
    response = await service_client.patch(
        f'/applications/{application["id"]}', json={'cv_id': new_cv['id']}, headers=intern.headers
    )
    assert response.status == 200
    assert response.json()['cv_id'] == new_cv['id']


async def test_intern_cannot_change_status(service_client, application_setup):
    application = application_setup['application']
    response = await service_client.patch(
        f'/applications/{application["id"]}', json={'status': 'approved'},
        headers=application_setup['intern'].headers,
    )
    assert response.status == 400


async def test_intern_cannot_update_other_intern_application(service_client, application_setup, register):
    mallory = await register('intern')
    application = application_setup['application']
    response = await service_client.patch(
        f'/applications/{application["id"]}', json={'cover_letter': 'hacked'}, headers=mallory.headers
    )
    assert response.status == 403

    own_view = await service_client.get('/applications/me', headers=application_setup['intern'].headers)
    assert own_view.json()[0]['cover_letter'] == 'Hire me'


async def test_intern_cannot_switch_to_other_intern_cv(service_client, application_setup, register, create_cv):
    foreign_cv = await create_cv(await register('intern'))
    application = application_setup['application']
    response = await service_client.patch(
        f'/applications/{application["id"]}', json={'cv_id': foreign_cv['id']},
        headers=application_setup['intern'].headers,
    )
    assert response.status == 404


# ---------------------------------------------------------------- delete


async def test_intern_withdraws_application(service_client, application_setup):
    intern = application_setup['intern']
    application = application_setup['application']
    response = await service_client.delete(f'/applications/{application["id"]}', headers=intern.headers)
    assert response.status == 204

    remaining = await service_client.get('/applications/me', headers=intern.headers)
    assert remaining.json() == []


async def test_intern_cannot_withdraw_other_intern_application(service_client, application_setup, register):
    mallory = await register('intern')
    application = application_setup['application']
    response = await service_client.delete(f'/applications/{application["id"]}', headers=mallory.headers)
    assert response.status == 403

    remaining = await service_client.get('/applications/me', headers=application_setup['intern'].headers)
    assert len(remaining.json()) == 1


async def test_withdraw_unknown_application(service_client, intern):
    response = await service_client.delete(f'/applications/{uuid.uuid4()}', headers=intern.headers)
    assert response.status == 404


async def test_recruiter_cannot_delete_application(service_client, application_setup):
    application = application_setup['application']
    response = await service_client.delete(
        f'/applications/{application["id"]}', headers=application_setup['recruiter'].headers
    )
    assert response.status == 403
