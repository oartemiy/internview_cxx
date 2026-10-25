# Start via `make test-debug` or `make test-release`


async def test_ping(service_client):
    response = await service_client.get('/ping')
    assert response.status == 200


async def test_status(service_client):
    response = await service_client.get('/status')
    assert response.status == 200
    assert response.json() == {'status': 'ok', 'service': 'InternView API', 'version': '0.0.1'}


async def test_unknown_route_is_404(service_client):
    response = await service_client.get('/definitely/not/a/route')
    assert response.status == 404
