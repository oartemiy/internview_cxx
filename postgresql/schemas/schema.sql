DROP SCHEMA IF EXISTS internview_schema CASCADE;

CREATE SCHEMA IF NOT EXISTS internview_schema;

CREATE TABLE IF NOT EXISTS internview_schema.users(
    id uuid PRIMARY KEY NOT NULL,
    login TEXT UNIQUE NOT NULL,
    password_hash text NOT NULL,
    name text NOT NULL,
    role TEXT NOT NULL,
    description text,
    profile_pic text,
    created_at timestamptz DEFAULT NOW(),
    CONSTRAINT user_role_check CHECK (role IN ('intern', 'recruiter'))
);

CREATE TABLE IF NOT EXISTS internview_schema.cvs(
    id uuid PRIMARY KEY NOT NULL,
    user_id uuid NOT NULL,
    title text NOT NULL,
    description text,
    cv_pdf text,
    created_at timestamptz DEFAULT NOW(),
    updated_at timestamptz DEFAULT NOW(),
    CONSTRAINT cv_user_title_unique UNIQUE (user_id, title)
);

CREATE TABLE IF NOT EXISTS internview_schema.vacancies(
    id uuid PRIMARY KEY NOT NULL,
    recruiter_id uuid NOT NULL,
    title text NOT NULL,
    description text,
    requirements text[],
    salary_range text,
    location text,
    work_mode text,
    experience_level text,
    is_active boolean NOT NULL DEFAULT TRUE,
    created_at timestamptz DEFAULT NOW(),
    updated_at timestamptz DEFAULT NOW(),
    CONSTRAINT vacancy_recruiter_title_unique UNIQUE (recruiter_id, title)
);

CREATE TABLE IF NOT EXISTS internview_schema.applications(
    id uuid PRIMARY KEY NOT NULL,
    vacancy_id uuid NOT NULL,
    intern_id uuid NOT NULL,
    cv_id uuid NOT NULL,
    status text NOT NULL DEFAULT 'pending',
    cover_letter text,
    applied_at timestamptz DEFAULT NOW(),
    updated_at timestamptz DEFAULT NOW(),
    CONSTRAINT application_intern_unique UNIQUE (vacancy_id, intern_id),
    CONSTRAINT check_status CHECK (status IN ('pending', 'reviewed', 'approved', 'rejected', 'cancelled'))
);

-- Added Foreign keys
ALTER TABLE internview_schema.cvs
    ADD CONSTRAINT fk_cv_user FOREIGN KEY (user_id) REFERENCES internview_schema.users(id) ON DELETE CASCADE;

ALTER TABLE internview_schema.vacancies
    ADD CONSTRAINT fk_vacancy_user FOREIGN KEY (recruiter_id) REFERENCES internview_schema.users(id) ON DELETE CASCADE;

ALTER TABLE internview_schema.applications
    ADD CONSTRAINT fk_application_vacancy FOREIGN KEY (vacancy_id) REFERENCES internview_schema.vacancies(id) ON DELETE CASCADE;

ALTER TABLE internview_schema.applications
    ADD CONSTRAINT fk_application_intern FOREIGN KEY (intern_id) REFERENCES internview_schema.users(id) ON DELETE CASCADE;

ALTER TABLE internview_schema.applications
    ADD CONSTRAINT fk_application_cv FOREIGN KEY (cv_id) REFERENCES internview_schema.cvs(id) ON DELETE CASCADE;

-- Create an index on 'internview_schema.cvs' for better query performance
CREATE INDEX IF NOT EXISTS idx_cvs_user_id ON internview_schema.cvs(user_id);

CREATE INDEX IF NOT EXISTS idx_vacancies_recruiter_id ON internview_schema.vacancies(recruiter_id);

CREATE INDEX IF NOT EXISTS idx_vacancies_is_active ON internview_schema.vacancies(is_active);

CREATE INDEX IF NOT EXISTS idx_vacancies_created_at ON internview_schema.vacancies(created_at DESC);

CREATE INDEX IF NOT EXISTS idx_applications_vacancy_id ON internview_schema.applications(vacancy_id);

CREATE INDEX IF NOT EXISTS idx_applications_intern_id ON internview_schema.applications(intern_id);

CREATE INDEX IF NOT EXISTS idx_applications_cv_id ON internview_schema.applications(cv_id);

CREATE INDEX IF NOT EXISTS idx_applications_status ON internview_schema.applications(status);

