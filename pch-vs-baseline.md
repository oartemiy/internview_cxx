# internview build-time profile: baseline vs PCH

Дата: 2026-10-24 · Release · Ninja · `JOBS=4` · ccache отключён · Apple clang 21.0.0

Методика: каждый прогон — чистый configure + `ninja internview` с `-ftime-trace`;
frontend/backend — суммы ClangBuildAnalyzer по всем трассированным компиляциям.

- baseline: `results-quick/baseline/`
- PCH: `results-quick/pch/baseline/` (`-DUSE_PCH=ON`)

## Сводка

| метрика | baseline | PCH | изменение | ускорение |
|---|---:|---:|---:|---:|
| TU скомпилировано | 51 | 53 | +2 | — |
| frontend, s | 58.8 | 29.0 | -50.7% | 2.03× |
| backend, s | 9.3 | 9.3 | +0.0% | 1.00× |
| compile CPU total, s | 68.1 | 38.3 | -43.8% | 1.78× |
| wall clock clean build, s | 193.76 | 186.04 | -4.0% | 1.04× |
| link `internview`, ms | 185 | 162 | -12.4% | 1.14× |

Отрицательное изменение = быстрее. `×` = во сколько раз быстрее PCH.

## Фазы clean build (.ninja_log)

| фаза | baseline, ms | PCH, ms | изменение |
|---|---:|---:|---:|
| компиляция завершена | 193531 | 185842 | -4.0% |
| линковка началась | 193531 | 185842 | -4.0% |
| линковка `internview` | 185 | 162 | -12.4% |
| весь build | 193716 | 186004 | -4.0% |

## Wall clock по прогонам, s

| прогон | baseline | PCH | изменение |
|---|---:|---:|---:|
| 1 | 193.76 | 186.04 | -4.0% |
| **медиана** | **193.76** | **186.04** | **-4.0%** |

## Инкрементальная пересборка проекта

Сценарий: все `src/*.cpp|hpp` «изменены» (`touch`, кроме `pch_userver.hpp`), затем
`cmake --build <build> -j 4 --target internview`. 3 прогона на конфигурацию, ccache off.

| прогон | baseline, s | PCH, s | изменение |
|---|---:|---:|---:|
| 1 | 20.91 | 12.76 | -39.0% |
| 2 | 20.74 | 12.69 | -38.8% |
| 3 | 20.85 | 12.58 | -39.7% |
| **медиана** | **20.85** | **12.69** | **-39.1%** |

Ускорение: **1.64×** (-39.1%).

## Translation units по frontend-времени (медиана, ms)

| TU | baseline | PCH | изменение | backend baseline | backend PCH |
|---|---:|---:|---:|---:|---:|
| `src/main.cpp` | 1685.7 | 930.2 | -44.8% | 847.5 | 837.0 |
| `src/storages/postgres/postgres_vacancy_storage.cpp` | 1660.1 | 1003.3 | -39.6% | 602.6 | 580.7 |
| `src/storages/postgres/postgres_user_storage.cpp` | 1536.0 | 930.7 | -39.4% | 433.3 | 425.3 |
| `src/storages/postgres/postgres_application_storage.cpp` | 1497.9 | 857.3 | -42.8% | 409.8 | 379.0 |
| `src/storages/postgres/postgres_cv_storage.cpp` | 1495.7 | 882.9 | -41.0% | 419.1 | 407.1 |
| `src/storages/postgres/postgres_refresh_token_storage.cpp` | 1426.1 | 826.6 | -42.0% | 296.4 | 265.9 |
| `src/components/storages/user_storage_component.cpp` | 1331.2 | 752.0 | -43.5% | 50.4 | 51.1 |
| `src/auth/auth_checker.cpp` | 1290.2 | 568.9 | -55.9% | 70.7 | 70.2 |
| `src/handlers/status/handler_status_get.cpp` | 1231.4 | 484.6 | -60.6% | 36.2 | 36.3 |
| `src/handlers/auth/handler_auth_register_post.cpp` | 1221.7 | 495.8 | -59.4% | 128.7 | 132.3 |
| `src/handlers/vacancy/handler_vacancy_applications_get.cpp` | 1210.9 | 555.2 | -54.2% | 175.0 | 190.8 |
| `src/services/auth_service.cpp` | 1207.8 | 645.1 | -46.6% | 527.5 | 531.6 |
| `src/handlers/vacancy/handler_vacancies_update_id.cpp` | 1206.2 | 557.2 | -53.8% | 253.5 | 252.0 |
| `src/services/jwt_service.cpp` | 1202.9 | 660.1 | -45.1% | 985.6 | 961.6 |
| `src/handlers/user/handler_user_delete.cpp` | 1201.8 | 546.6 | -54.5% | 111.5 | 111.4 |
| `src/handlers/application/handler_application_update.cpp` | 1196.5 | 582.0 | -51.4% | 190.0 | 194.8 |
| `src/handlers/vacancy/handler_vacancies_toggle.cpp` | 1194.0 | 537.4 | -55.0% | 165.0 | 165.3 |
| `src/handlers/vacancy/handler_vacancies_delete.cpp` | 1190.3 | 532.2 | -55.3% | 120.8 | 122.8 |
| `src/handlers/auth/handler_auth_change_password_post.cpp` | 1187.9 | 495.1 | -58.3% | 60.5 | 59.8 |
| `src/handlers/auth/handler_auth_login_post.cpp` | 1187.4 | 488.7 | -58.8% | 76.7 | 74.9 |
| `src/handlers/user/handler_user_update.cpp` | 1187.3 | 506.6 | -57.3% | 137.8 | 138.8 |
| `src/handlers/application/handler_application_post.cpp` | 1184.6 | 565.5 | -52.3% | 185.9 | 184.2 |
| `src/handlers/files/handler_profile_pic_get_for_recruiter.cpp` | 1179.8 | 518.5 | -56.1% | 137.5 | 160.4 |
| `src/handlers/vacancy/handler_vacancy_post.cpp` | 1179.1 | 546.3 | -53.7% | 216.7 | 217.7 |
| `src/handlers/vacancy/handler_vacancies_get_me.cpp` | 1178.2 | 544.2 | -53.8% | 143.4 | 144.2 |
| `src/handlers/files/handler_profile_pic_post.cpp` | 1169.7 | 492.1 | -57.9% | 66.3 | 64.3 |
| `src/handlers/user/handler_user_get_by_id.cpp` | 1164.5 | 539.5 | -53.7% | 179.0 | 182.2 |
| `src/handlers/vacancy/handler_vacancies_get.cpp` | 1163.2 | 533.7 | -54.1% | 119.6 | 121.7 |
| `src/handlers/files/handler_cv_pdf_get.cpp` | 1161.9 | 514.1 | -55.8% | 102.2 | 103.1 |
| `src/handlers/vacancy/handler_vacancies_get_by_id.cpp` | 1160.9 | 515.7 | -55.6% | 119.5 | 119.9 |
| `src/handlers/cv/handler_cv_update.cpp` | 1160.6 | 513.5 | -55.8% | 161.3 | 163.8 |
| `src/handlers/cv/handler_cv_get_by_id.cpp` | 1159.6 | 516.9 | -55.4% | 118.1 | 119.2 |
| `src/handlers/auth/handler_auth_logout_post.cpp` | 1157.3 | 484.0 | -58.2% | 50.0 | 50.6 |
| `src/handlers/cv/handler_cv_delete.cpp` | 1156.7 | 512.9 | -55.7% | 121.2 | 123.5 |
| `src/handlers/files/handler_profile_pic_get.cpp` | 1156.1 | 494.2 | -57.3% | 64.4 | 65.4 |
| `src/handlers/files/handler_cv_pdf_post.cpp` | 1155.2 | 512.8 | -55.6% | 133.8 | 135.0 |
| `src/handlers/user/handler_user_get.cpp` | 1154.6 | 493.7 | -57.2% | 94.0 | 89.6 |
| `src/handlers/application/handler_application_delete.cpp` | 1154.1 | 510.5 | -55.8% | 118.3 | 120.7 |
| `src/handlers/cv/handler_cv_get.cpp` | 1151.7 | 532.9 | -53.7% | 125.3 | 124.9 |
| `src/handlers/auth/handler_auth_refresh_post.cpp` | 1150.4 | 491.0 | -57.3% | 46.7 | 47.6 |
| `src/handlers/cv/handler_cv_post.cpp` | 1148.7 | 530.3 | -53.8% | 124.1 | 128.0 |
| `src/handlers/application/handler_applications_get.cpp` | 1143.9 | 525.6 | -54.1% | 94.0 | 94.5 |
| `src/components/services/auth_service_component.cpp` | 1032.4 | 471.8 | -54.3% | 94.7 | 108.5 |
| `src/storages/local/local_file_storage.cpp` | 830.4 | 252.6 | -69.6% | 143.3 | 144.5 |
| `src/components/storages/refresh_token_storage_component.cpp` | 809.7 | 290.6 | -64.1% | 86.5 | 90.2 |
| `src/components/storages/pdf_storage_component.cpp` | 787.8 | 208.5 | -73.5% | 130.6 | 122.8 |
| `src/components/storages/vacancy_storage_component.cpp` | 785.9 | 275.7 | -64.9% | 33.7 | 30.5 |
| `src/components/storages/img_storage_component.cpp` | 766.5 | 210.0 | -72.6% | 124.9 | 126.0 |
| `src/components/storages/cv_storage_component.cpp` | 744.1 | 252.5 | -66.1% | 30.6 | 32.2 |
| `src/components/storages/application_storage_component.cpp` | 737.8 | 242.1 | -67.2% | 34.8 | 30.6 |
| `src/utils/password.cpp` | 320.6 | 99.2 | -69.0% | 17.4 | 18.1 |
| `cmake_pch.hxx` (генерация PCH, 2 шт.) | — | 985.0 | — | — | 0.0 |

## Топ заголовков по inclusive parse time (медиана, ms)

| заголовок | baseline | PCH | изменение |
|---|---:|---:|---:|
| `src/services/auth_service.hpp` | 9669 | 5885 | -39.1% |
| `src/components/storages/application_storage_component.hpp` | 5266 | 1193 | -77.3% |
| `src/components/storages/vacancy_storage_component.hpp` | 4283 | 899 | -79.0% |
| `src/components/storages/cv_storage_component.hpp` | 3518 | 842 | -76.1% |
| `src/components/storages/user_storage_component.hpp` | 3233 | 1021 | -68.4% |
| `.cache/cpm/userver/47ed/postgresql/include/userver/storages/postgres/cluster.hpp` | 3072 | 1945 | -36.7% |
| `src/components/services/auth_service_component.hpp` | 1886 | 771 | -59.1% |
| `src/storages/postgres/postgres_user_storage.hpp` | 1552 | 786 | -49.4% |
| `src/auth/auth_checker.hpp` | 1346 | — | — |
| `src/handlers/status/handler_status_get.hpp` | 1179 | — | — |

`—` = заголовок не попал в top-10 PCH-прогона (в summary.json хранится только top-10).

## Выводы

- PCH режет frontend-время кода проекта примерно вдвое (58.8 → 29.0 s, −50.7%).
- backend (codegen) не меняется: 9.3 s в обоих случаях.
- На инкрементальной пересборке проекта выигрыш **1.64×** (20.85 → 12.69 s).
- На полном clean build wall падает лишь на 4% (193.8 → 186.0 s), потому что
  основное время занимают ~900 TU самого userver, на которые PCH не влияет.
- Накладные расходы: 2 PCH-трансляции (~2 s) и файл `.pch` ~35 MB на таргет.

## Как воспроизвести

```bash
export CPM_SOURCE_CACHE=$PWD/.cache/cpm
export CCACHE_DISABLE=1
OPTS='-DUSERVER_PYTHON_PATH=/opt/homebrew/bin/python3.12 \
  -DUSERVER_CHECK_PACKAGE_VERSIONS=0 \
  -DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl@3 \
  -DCMAKE_CXX_FLAGS=-I'"$PWD"'/.cache/openssl3-include'

# baseline
RESULTS_ROOT=results-quick BUILD_DIR=$PWD/build-cba-baseline \
  REPS=1 JOBS=4 CMAKE_OPTS="$OPTS" scripts/run_all.sh

# PCH
RESULTS_ROOT=results-quick/pch BUILD_DIR=$PWD/build-cba-pch \
  REPS=1 JOBS=4 CMAKE_OPTS="$OPTS -DUSE_PCH=ON" scripts/run_all.sh
```

Инкрементальный сценарий: `touch` всех `src/*.cpp|hpp` (кроме `src/pch_userver.hpp`), затем
`cmake --build <build> -j 4 --target internview`, 3 прогона на конфигурацию.
