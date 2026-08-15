DROP SCHEMA IF EXISTS internview_schema CASCADE;

CREATE SCHEMA IF NOT EXISTS internview_schema;

CREATE TABLE IF NOT EXISTS internview_schema.user(
    id UUID PRIMARY KEY NOT NULL,
    login TEXT UNIQUE NOT NULL,
    password_hash text NOT NULL,
    name text NOT NULL,
    role TEXT NOT NULL,
    description TEXT,
    profile_pic TEXT,
    created_at timestamptz DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS internview_schema.cv(
    id UUID PRIMARY KEY NOT NULL,
    user_id UUID NOT NULL,
    title TEXT NOT NULL,
    description TEXT,
    cv_pdf TEXT,
    created_at timestamptz DEFAULT NOW(),
    updated_at timestamptz DEFAULT NOW(),
    CONSTRAINT fk_cv_user FOREIGN KEY (user_id) REFERENCES internview_schema.user(id) ON DELETE CASCADE
);

-- Create an index on 'internview_schema.cv' for better query performance
CREATE INDEX idx_cv_user_id ON internview_schema.cv(user_id);

