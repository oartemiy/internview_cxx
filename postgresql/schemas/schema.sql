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
    created_at timestamptz DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS internview_schema.cvs(
    id uuid PRIMARY KEY NOT NULL,
    user_id uuid NOT NULL,
    title text NOT NULL,
    description text,
    cv_pdf text,
    created_at timestamptz DEFAULT NOW(),
    updated_at timestamptz DEFAULT NOW(),
    -- CONSTRAINT fk_cv_user FOREIGN KEY (user_id) REFERENCES internview_schema.users(id) ON DELETE CASCADE,
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
    is_active boolean DEFAULT TRUE,
    created_at timestamptz DEFAULT NOW(),
    updated_at timestamptz DEFAULT NOW(),
    -- CONSTRAINT fk_vacancy_user FOREIGN KEY (recruiter_id) REFERENCES internview_schema.users(id) ON DELETE CASCADE,
    CONSTRAINT vacancy_recruiter_title_unique UNIQUE (recruiter_id, title)
);

-- Create an index on 'internview_schema.cvs' for better query performance
CREATE INDEX IF NOT EXISTS idx_cvs_user_id ON internview_schema.cvs(user_id);

CREATE INDEX IF NOT EXISTS idx_vacancies_recruiter_id ON internview_schema.vacancies(recruiter_id);

CREATE INDEX IF NOT EXISTS idx_vacancies_is_active ON internview_schema.vacancies(is_active);

