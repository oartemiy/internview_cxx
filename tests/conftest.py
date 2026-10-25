import dataclasses
import uuid

import pytest

from testsuite.databases.pgsql import discover

pytest_plugins = [
    'pytest_userver.plugins.core',
    'pytest_userver.plugins.postgresql',
]

USERVER_CONFIG_HOOKS = ['userver_config_uploads']

DB_NAME = 'schema'

PNG_BYTES = b'\x89PNG\r\n\x1a\n' + b'\x00' * 32
PDF_BYTES = b'%PDF-1.4\n%fake pdf for tests\n%%EOF\n'


@pytest.fixture(scope='session')
def initial_data_path(service_source_dir):
    """Path for find files with data"""
    return [
        service_source_dir / 'postgresql/data',
    ]


@pytest.fixture(scope='session')
def pgsql_local(service_source_dir, pgsql_local_create):
    """Create schemas databases for tests"""
    databases = discover.find_schemas(
        'internview',
        [service_source_dir.joinpath('postgresql/schemas')],
    )
    return pgsql_local_create(list(databases.values()))


@pytest.fixture(scope='session')
def service_secdist_path(service_source_dir):
    """Tests must not depend on the developer's gitignored configs/secure_data.json"""
    return service_source_dir / 'tests' / 'static' / 'secure_data.json'


@pytest.fixture(scope='session')
def uploads_dir(tmp_path_factory):
    return tmp_path_factory.mktemp('uploads')


@pytest.fixture(scope='session')
def userver_config_uploads(uploads_dir):
    """Store uploaded files in a temporary directory instead of the repo"""

    def patch(config_yaml, config_vars):
        components = config_yaml['components_manager']['components']
        components['img-storage']['base-dir'] = str(uploads_dir / 'img')
        components['pdf-storage']['base-dir'] = str(uploads_dir / 'pdf')

    return patch


@dataclasses.dataclass
class AuthUser:
    id: str
    login: str
    password: str
    role: str
    access_token: str
    refresh_token: str

    @property
    def headers(self):
        return {'Authorization': f'Bearer {self.access_token}'}


@pytest.fixture
def register(service_client):
    async def _register(role='intern', *, login=None, password='password123', name='Test User', **extra):
        login = login or f'{role}-{uuid.uuid4().hex[:8]}'
        response = await service_client.post(
            '/auth/register',
            json={'login': login, 'password': password, 'name': name, 'role': role, **extra},
        )
        assert response.status == 200, response.text
        body = response.json()
        return AuthUser(
            id=body['id'],
            login=login,
            password=password,
            role=role,
            access_token=body['access_token'],
            refresh_token=body['refresh_token'],
        )

    return _register


@pytest.fixture
async def intern(register):
    return await register('intern')


@pytest.fixture
async def recruiter(register):
    return await register('recruiter')


@pytest.fixture
def create_cv(service_client):
    async def _create_cv(user, title=None, **extra):
        response = await service_client.post(
            '/cvs',
            json={'title': title or f'CV {uuid.uuid4().hex[:8]}', **extra},
            headers=user.headers,
        )
        assert response.status == 201, response.text
        return response.json()

    return _create_cv


@pytest.fixture
def create_vacancy(service_client):
    async def _create_vacancy(user, title=None, **extra):
        response = await service_client.post(
            '/vacancies',
            json={'title': title or f'Vacancy {uuid.uuid4().hex[:8]}', **extra},
            headers=user.headers,
        )
        assert response.status == 201, response.text
        return response.json()

    return _create_vacancy


@pytest.fixture
def apply(service_client):
    async def _apply(user, vacancy_id, cv_id, cover_letter=None):
        payload = {'vacancy_id': vacancy_id, 'cv_id': cv_id}
        if cover_letter is not None:
            payload['cover_letter'] = cover_letter
        response = await service_client.post('/applications', json=payload, headers=user.headers)
        assert response.status == 201, response.text
        return response.json()

    return _apply


@pytest.fixture
async def application_setup(intern, recruiter, create_cv, create_vacancy, apply):
    """Intern applied with a CV to a recruiter's vacancy"""
    cv = await create_cv(intern)
    vacancy = await create_vacancy(recruiter)
    application = await apply(intern, vacancy['id'], cv['id'], cover_letter='Hire me')
    return {
        'intern': intern,
        'recruiter': recruiter,
        'cv': cv,
        'vacancy': vacancy,
        'application': application,
    }
