# InternView API Reference

REST API for the InternView internship recruiting platform (userver C++ framework, PostgreSQL, JWT auth).

- **Base URL:** `http://localhost:8080`
- **Content type:** `application/json` (file upload/download endpoints excepted)
- **Build & run:** see [BUILDRUN.md](BUILDRUN.md). Quick start: `make cmake-debug && make build-debug && make start-debug`

For build/run details see [BUILDRUN.md](BUILDRUN.md); this document describes only the HTTP API.

---

## Table of Contents

- [InternView API Reference](#internview-api-reference)
  - [Table of Contents](#table-of-contents)
  - [Authentication](#authentication)
  - [Errors](#errors)
  - [Health \& Status](#health--status)
    - [`GET /ping`](#get-ping)
    - [`GET /status`](#get-status)
  - [Auth endpoints](#auth-endpoints)
    - [`POST /auth/register`](#post-authregister)
    - [`POST /auth/login`](#post-authlogin)
    - [`POST /auth/change-password`](#post-authchange-password)
  - [Users](#users)
    - [`GET /users/me`](#get-usersme)
    - [`PATCH /users/me`](#patch-usersme)
    - [`DELETE /users/me`](#delete-usersme)
    - [`GET /users/{id}`](#get-usersid)
    - [Profile pictures](#profile-pictures)
  - [CVs](#cvs)
    - [`POST /cvs`](#post-cvs)
    - [`GET /cvs/me`](#get-cvsme)
    - [`GET /cvs/{id}`](#get-cvsid)
    - [`PATCH /cvs/{id}`](#patch-cvsid)
    - [`DELETE /cvs/{id}`](#delete-cvsid)
    - [CV PDF files](#cv-pdf-files)
  - [Vacancies](#vacancies)
    - [`POST /vacancies`](#post-vacancies)
    - [`GET /vacancies`](#get-vacancies)
    - [`GET /vacancies/{id}`](#get-vacanciesid)
    - [`GET /vacancies/me`](#get-vacanciesme)
    - [`PATCH /vacancies/{id}`](#patch-vacanciesid)
    - [`DELETE /vacancies/{id}`](#delete-vacanciesid)
    - [`PATCH /vacancies/{id}/toggle`](#patch-vacanciesidtoggle)
    - [`GET /vacancies/{id}/applications`](#get-vacanciesidapplications)
  - [Applications](#applications)
    - [`POST /applications`](#post-applications)
    - [`GET /applications/me`](#get-applicationsme)
    - [`PATCH /applications/{id}`](#patch-applicationsid)
    - [`DELETE /applications/{id}`](#delete-applicationsid)
  - [Status Codes](#status-codes)

---

## Authentication

All protected endpoints require a JWT in the `Authorization` header:

```
Authorization: Bearer <token>
```

Tokens are issued by `/auth/register` and `/auth/login`, are signed with **HS256**, contain the `user_id` and `role` claims, and expire after **24 hours**.

There are two user roles: `intern` and `recruiter`. Role-sensitive endpoints return `400` when called by a user with the wrong role.

## Errors

All business errors use the same JSON shape:

```json
{"message": "<reason>"}
```

| Code | Meaning |
|---|---|
| `400` | Validation failure, wrong role, ownership violation |
| `401` | Missing/invalid/expired token or wrong credentials |

---

## Health & Status

### `GET /ping`

Liveness probe. Returns plain text.

### `GET /status`

```json
{
    "status": "ok",
    "service": "InternView API",
    "version": "0.0.1"
}
```

---

## Auth endpoints

### `POST /auth/register`

Creates an account and returns it together with a fresh JWT.

**Request**

```json
{
    "login": "anna",            // required; "me" is reserved
    "password": "secret123",    // required, min length 2
    "name": "Anna",             // required
    "role": "intern",           // required: "intern" | "recruiter"
    "description": "CS student" // optional
}
```

**Response `200`**

```json
{
    "id": "7b9e...-uuid",
    "login": "anna",
    "name": "Anna",
    "role": "intern",
    "description": "CS student",
    "profile_pic": null,
    "created_at": "2026-08-25T12:00:00.000000+00:00",
    "token": "eyJhbGciOiJIUzI1NiIs..."
}
```

### `POST /auth/login`

**Request**

```json
{
    "login": "anna",
    "password": "secret123"
}
```

**Response `200`** — same shape as register (including `token`). Wrong credentials → `400`.

### `POST /auth/change-password`

🔒 Requires authorization.

**Request**

```json
{
    "old_password": "secret123",
    "new_password": "newsecret456"
}
```

**Response `200`**

```json
{"status": "password changed"}
```

Old password mismatch → `400 {"message": "Password is incorrect"}`.

---

## Users

The `User` object returned by all user endpoints:

```json
{
    "id": "7b9e...-uuid",
    "login": "anna",
    "name": "Anna",
    "role": "intern",
    "description": null,
    "profile_pic": null,
    "created_at": "2026-08-25T12:00:00.000000+00:00"
}
```

(`profile_pic` holds the stored file name; fetch the actual image via the profile-pic endpoints below.)

### `GET /users/me`

🔒 Returns the authenticated user.

### `PATCH /users/me`

🔒 Partial update of public info. Only provided fields are changed. The `role` is immutable.

```json
{
    "login": "anna_dev",        // optional; cannot be "me"
    "name": "Anna K.",          // optional
    "description": "C++ intern" // optional
}
```

Response `200` — updated user object.

### `DELETE /users/me`

🔒 Deletes the account after re-checking credentials. Also cascades deletion of the user's CVs (intern) or vacancies (recruiter) and related applications.

```json
{
    "login": "anna",
    "password": "secret123"
}
```

**Response `200`**

```json
{
    "status": "success",
    "deleted user": "anna"
}
```

### `GET /users/{id}`

🔒 Restricted visibility. Allowed only if:

- `{id}` is your own id, **or**
- you are a `recruiter` and the target intern applied to one of your vacancies.

Otherwise → `400`. Response — user object.

### Profile pictures

Upload is `multipart/form-data` with a single field **`file`** (max size 5 MB).

| Method & path | Auth | Notes |
|---|---|---|
| `POST /users/me/profile-pic` | 🔒 any | Upload; returns `"Uploaded"` (400 `"Empty file"` if empty) |
| `GET /users/me/profile-pic` | 🔒 any | Returns raw image (`image/png` / `image/jpeg`) |
| `GET /users/{id}/profile-pic` | 🔒 recruiter | Only for interns who applied to caller's vacancies, else `400` |

Example upload:

```bash
curl -X POST http://localhost:8080/users/me/profile-pic \
     -H "Authorization: Bearer $TOKEN" \
     -F "file=@photo.png"
```

---

## CVs

Owned by **interns**. The `CV` object:

```json
{
    "id": "3fa8...-uuid",
    "user_id": "7b9e...-uuid",
    "title": "Backend intern CV",
    "description": "2 years of pet C++ projects",
    "cv_pdf": "3fa8....pdf",
    "created_at": "2026-08-25T12:10:00.000000+00:00",
    "updated_at": "2026-08-25T12:10:00.000000+00:00"
}
```

### `POST /cvs`

🔒 **intern only**

```json
{
    "title": "Backend intern CV",   // required
    "description": "...",           // optional
    "cv_pdf": "file-name.pdf"       // optional
}
```

Response `200` — created CV object. `(user_id, title)` must be unique per owner.

### `GET /cvs/me`

🔒 **intern only** — array of the caller's CVs.

### `GET /cvs/{id}`

🔒 Any authorized user. Recruiters may only fetch CVs attached to applications on their vacancies.

### `PATCH /cvs/{id}`

🔒 **intern owner** — partial update:

```json
{
    "title": "Senior Backend CV",   // optional
    "description": "updated text",  // optional
    "cv_pdf": "other.pdf"           // optional
}
```

Empty body → `400 {"message": "Empty request data body. Nothing to update"}`.
Response `200` — updated CV object.

### `DELETE /cvs/{id}`

🔒 **intern owner**

```json
{"status": "success", "deleted cv": "3fa8...-uuid"}
```

### CV PDF files

`multipart/form-data`, single field **`file`**.

| Method & path | Auth | Notes |
|---|---|---|
| `POST /cvs/{id}/pdf` | 🔒 intern owner | Upload; returns `"Uploaded"` |
| `GET /cvs/{id}/pdf` | 🔒 any* | Raw PDF (`application/pdf`; empty body if not uploaded) |

---

## Vacancies

Owned by **recruiters**. Listing and reading single vacancies are public. The `Vacancy` object:

```json
{
    "id": "c11d...-uuid",
    "recruiter_id": "99aa...-uuid",
    "title": "C++ Backend Intern",
    "description": "Work on our core service",
    "requirements": ["C++20", "SQL", "Linux"],
    "salary_range": "$1000-1500",
    "location": "Remote",
    "work_mode": "remote",
    "experience_level": "junior",
    "is_active": true,
    "created_at": "2026-08-25T09:00:00.000000+00:00",
    "updated_at": "2026-08-25T09:00:00.000000+00:00"
}
```

### `POST /vacancies`

🔒 **recruiter only**

```json
{
    "title": "C++ Backend Intern",                  // required
    "description": "Work on our core service",      // optional
    "requirements": ["C++20", "SQL"],               // optional, list of strings
    "salary_range": "$1000-1500",                   // optional
    "location": "Remote",                           // optional
    "work_mode": "remote",                          // optional
    "experience_level": "junior"                    // optional
}
```

Response `200` — created vacancy object (`is_active` defaults to `true`).

### `GET /vacancies`

🌐 **Public.** Paginated list of active vacancies, newest first.

Query parameters:

| Param | Default | Description |
|---|---|---|
| `limit` | `15` | Page size |
| `offset` | `0` | Skip count |

```bash
curl "http://localhost:8080/vacancies?limit=10&offset=20"
```

Response `200` — array of vacancy objects.

### `GET /vacancies/{id}`

🌐 **Public.** Single vacancy (active or archived).

### `GET /vacancies/me`

🔒 **recruiter only** — all vacancies of the caller.

### `PATCH /vacancies/{id}`

🔒 **recruiter owner** — partial update; any subset of the create-time fields.

### `DELETE /vacancies/{id}`

🔒 **recruiter owner**

```json
{"status": "success", "deleted vanancy": "c11d...-uuid"}
```

### `PATCH /vacancies/{id}/toggle`

🔒 **recruiter owner** — flips `is_active` (publish/unpublish). Response `200` — updated vacancy object.

### `GET /vacancies/{id}/applications`

🔒 **recruiter owning the vacancy** — array of application objects for that vacancy; otherwise `400`.

---

## Applications

An intern applies to a vacancy with one of their CVs. The `Application` object:

```json
{
    "id": "55f1...-uuid",
    "vacancy_id": "c11d...-uuid",
    "intern_id": "7b9e...-uuid",
    "cv_id": "3fa8...-uuid",
    "status": "pending",
    "cover_letter": "Hi, I would love to join...",
    "applied_at": "2026-08-25T13:30:00.000000+00:00",
    "updated_at": "2026-08-25T13:30:00.000000+00:00"
}
```

Status values: `pending` (initial), `reviewed`, `approved`, `rejected`, `cancelled`.
One application per `(vacancy, intern)` pair.

### `POST /applications`

🔒 **intern only**

```json
{
    "vacancy_id": "c11d...-uuid",   // required
    "cv_id": "3fa8...-uuid",        // required, must belong to the caller
    "cover_letter": "Hi, ..."       // optional
}
```

Response `200` — created application object.

### `GET /applications/me`

🔒 Any authorized user:

- **intern** → their own applications;
- **recruiter** → applications across all their vacancies.

### `PATCH /applications/{id}`

🔒 Role-dependent, mutually exclusive request bodies:

**intern** (own application) — may change only `cv_id` and/or `cover_letter`:

```json
{
    "cv_id": "another-cv-uuid",
    "cover_letter": "Updated letter"
}
```

**recruiter** (owner of the vacancy) — may change only `status`:

```json
{"status": "reviewed"}   // pending | reviewed | approved | rejected | cancelled
```

Mixing intern fields with `status` in one request → `400`. Response `200` — updated application object.

### `DELETE /applications/{id}`

🔒 **intern** (own application)

```json
{"status": "deleted", "deleted application": "55f1...-uuid"}
```

---

## Status Codes

| Code | When |
|---|---|
| `200` | Success (all endpoints) |
| `400` | Bad request body, wrong role, ownership violation, validation failure |
| `401` | Missing/malformed `Authorization` header, invalid or expired JWT |
| `404` | Unknown path (auto-generated by userver) |
| `405` | Wrong method for existing path (auto-generated by userver) |

Legend used above: 🌐 — no auth required · 🔒 — `Authorization: Bearer <JWT>` required.
