import base64
import hashlib
import hmac
import json
import time

import pytest

from conftest import DB_NAME


# ---------------------------------------------------------------- register


@pytest.mark.parametrize('role', ['intern', 'recruiter'])
async def test_register(service_client, role):
    response = await service_client.post(
        '/auth/register',
        json={'login': 'alice', 'password': 'password123', 'name': 'Alice', 'role': role, 'description': 'hi'},
    )
    assert response.status == 200
    body = response.json()
    assert body['login'] == 'alice'
    assert body['name'] == 'Alice'
    assert body['role'] == role
    assert body['description'] == 'hi'
    assert body['profile_pic'] is None
    assert body['id']
    assert body['created_at']
    assert body['access_token']
    assert body['refresh_token']
    assert 'password' not in body
    assert 'password_hash' not in body


async def test_register_rejects_unknown_role(service_client):
    response = await service_client.post(
        '/auth/register',
        json={'login': 'alice', 'password': 'password123', 'name': 'Alice', 'role': 'admin'},
    )
    assert response.status == 400


async def test_register_rejects_reserved_login_me(service_client):
    response = await service_client.post(
        '/auth/register',
        json={'login': 'me', 'password': 'password123', 'name': 'Me', 'role': 'intern'},
    )
    assert response.status == 400


@pytest.mark.parametrize(
    'field, value',
    [
        ('password', '12345'),
        ('login', 'ab'),
        ('login', 'a' * 51),
        ('name', ''),
        ('name', 'n' * 101),
    ],
)
async def test_register_validates_lengths(service_client, field, value):
    payload = {'login': 'alice', 'password': 'password123', 'name': 'Alice', 'role': 'intern'}
    payload[field] = value
    response = await service_client.post('/auth/register', json=payload)
    assert response.status == 400


async def test_register_length_is_counted_in_characters(service_client):
    response = await service_client.post(
        '/auth/register',
        json={'login': 'юля', 'password': 'пароль', 'name': 'Ю' * 100, 'role': 'intern'},
    )
    assert response.status == 200


async def test_register_ignores_profile_pic(service_client):
    response = await service_client.post(
        '/auth/register',
        json={'login': 'alice', 'password': 'password123', 'name': 'A', 'role': 'intern', 'profile_pic': 'x.png'},
    )
    assert response.status == 200
    assert response.json()['profile_pic'] is None


async def test_register_duplicate_login_is_conflict(service_client, register):
    await register('intern', login='alice')
    response = await service_client.post(
        '/auth/register',
        json={'login': 'alice', 'password': 'password123', 'name': 'Other', 'role': 'recruiter'},
    )
    assert response.status == 409


async def test_register_missing_required_field_is_bad_request(service_client):
    response = await service_client.post(
        '/auth/register',
        json={'login': 'alice', 'name': 'Alice', 'role': 'intern'},
    )
    assert response.status == 400


async def test_register_stores_password_hashed(service_client, register, pgsql):
    user = await register('intern', password='password123')
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute('SELECT password_hash FROM internview_schema.users WHERE id = %s', (user.id,))
    (password_hash,) = cursor.fetchone()
    assert password_hash != 'password123'
    assert password_hash.startswith('$argon2id$')


# ---------------------------------------------------------------- login


async def test_login(service_client, register):
    user = await register('recruiter', login='bob', password='password123')
    response = await service_client.post('/auth/login', json={'login': 'bob', 'password': 'password123'})
    assert response.status == 200
    body = response.json()
    assert body['id'] == user.id
    assert body['role'] == 'recruiter'
    assert body['access_token']
    assert body['refresh_token']

    me = await service_client.get('/users/me', headers={'Authorization': f'Bearer {body["access_token"]}'})
    assert me.status == 200
    assert me.json()['id'] == user.id


async def test_login_wrong_password(service_client, register):
    await register('intern', login='bob', password='password123')
    response = await service_client.post('/auth/login', json={'login': 'bob', 'password': 'wrong-password'})
    assert response.status == 401


async def test_login_unknown_user(service_client):
    response = await service_client.post('/auth/login', json={'login': 'ghost', 'password': 'password123'})
    assert response.status == 401


async def test_refresh_tokens_are_stored_hashed(service_client, register, pgsql):
    user = await register('intern')
    cursor = pgsql[DB_NAME].cursor()
    cursor.execute('SELECT token_hash FROM internview_schema.refresh_tokens WHERE user_id = %s', (user.id,))
    hashes = [row[0] for row in cursor]
    assert len(hashes) == 1
    assert hashes[0] != user.refresh_token


# ---------------------------------------------------------------- authorization


async def test_protected_endpoint_without_token(service_client):
    response = await service_client.get('/users/me')
    assert response.status == 401


async def test_protected_endpoint_with_non_bearer_scheme(service_client):
    response = await service_client.get('/users/me', headers={'Authorization': 'Basic YWxpY2U6cGFzcw=='})
    assert response.status == 401


def _make_hs256_jwt(payload, secret):
    def b64(data):
        return base64.urlsafe_b64encode(data).rstrip(b'=').decode()

    header = b64(json.dumps({'alg': 'HS256', 'typ': 'JWT'}).encode())
    body = b64(json.dumps(payload).encode())
    signature = hmac.new(secret.encode(), f'{header}.{body}'.encode(), hashlib.sha256).digest()
    return f'{header}.{body}.{b64(signature)}'


async def test_protected_endpoint_with_token_signed_by_other_key(service_client, intern):
    token = _make_hs256_jwt(
        {
            'iss': 'internview',
            'exp': int(time.time()) + 600,
            'user_id': intern.id,
            'role': 'recruiter',
            'password_version': '0',
        },
        'attacker-secret',
    )
    response = await service_client.get('/users/me', headers={'Authorization': f'Bearer {token}'})
    assert response.status == 401


@pytest.mark.parametrize('token', ['garbage', 'a.b.c', 'eyJhbGciOiJIUzI1NiJ9.e30'])
async def test_protected_endpoint_with_malformed_token(service_client, token):
    response = await service_client.get('/users/me', headers={'Authorization': f'Bearer {token}'})
    assert response.status == 401


# ---------------------------------------------------------------- refresh / logout


async def test_refresh_returns_working_tokens(service_client, intern):
    response = await service_client.post('/auth/refresh', json={'refresh_token': intern.refresh_token})
    assert response.status == 200
    tokens = response.json()
    assert tokens['refresh_token'] != intern.refresh_token

    me = await service_client.get('/users/me', headers={'Authorization': f'Bearer {tokens["access_token"]}'})
    assert me.status == 200

    again = await service_client.post('/auth/refresh', json={'refresh_token': tokens['refresh_token']})
    assert again.status == 200


async def test_refresh_token_is_single_use(service_client, intern):
    first = await service_client.post('/auth/refresh', json={'refresh_token': intern.refresh_token})
    assert first.status == 200
    reused = await service_client.post('/auth/refresh', json={'refresh_token': intern.refresh_token})
    assert reused.status == 401


async def test_refresh_with_unknown_token(service_client):
    response = await service_client.post('/auth/refresh', json={'refresh_token': 'not-a-real-token'})
    assert response.status == 401


async def test_logout_revokes_refresh_tokens(service_client, intern):
    response = await service_client.post('/auth/logout', headers=intern.headers)
    assert response.status == 200

    refresh = await service_client.post('/auth/refresh', json={'refresh_token': intern.refresh_token})
    assert refresh.status == 401


async def test_logout_requires_auth(service_client):
    response = await service_client.post('/auth/logout')
    assert response.status == 401


# ---------------------------------------------------------------- change password


async def test_change_password(service_client, intern):
    response = await service_client.post(
        '/auth/change-password',
        json={'old_password': intern.password, 'new_password': 'new-password'},
        headers=intern.headers,
    )
    assert response.status == 204
    assert response.content == b''

    # old access token is rejected right away
    me = await service_client.get('/users/me', headers=intern.headers)
    assert me.status == 401

    # all refresh tokens are revoked
    refresh = await service_client.post('/auth/refresh', json={'refresh_token': intern.refresh_token})
    assert refresh.status == 401

    old_login = await service_client.post('/auth/login', json={'login': intern.login, 'password': intern.password})
    assert old_login.status == 401

    new_login = await service_client.post('/auth/login', json={'login': intern.login, 'password': 'new-password'})
    assert new_login.status == 200
    new_token = new_login.json()['access_token']
    me = await service_client.get('/users/me', headers={'Authorization': f'Bearer {new_token}'})
    assert me.status == 200


async def test_change_password_with_wrong_old_password(service_client, intern):
    response = await service_client.post(
        '/auth/change-password',
        json={'old_password': 'wrong-password', 'new_password': 'new-password'},
        headers=intern.headers,
    )
    assert response.status == 403

    me = await service_client.get('/users/me', headers=intern.headers)
    assert me.status == 200
    login = await service_client.post('/auth/login', json={'login': intern.login, 'password': intern.password})
    assert login.status == 200


async def test_change_password_validates_new_password(service_client, intern):
    response = await service_client.post(
        '/auth/change-password',
        json={'old_password': intern.password, 'new_password': '12345'},
        headers=intern.headers,
    )
    assert response.status == 400


async def test_refresh_without_token_field(service_client):
    response = await service_client.post('/auth/refresh', json={})
    assert response.status == 400


async def test_change_password_requires_auth(service_client):
    response = await service_client.post(
        '/auth/change-password',
        json={'old_password': 'a', 'new_password': 'b'},
    )
    assert response.status == 401
