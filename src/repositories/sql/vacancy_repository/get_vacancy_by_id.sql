-- Select all rows from 'internview_schema.vacancies'
SELECT
    id,
    recruiter_id,
    title,
    description,
    requirements,
    salary_range,
    location,
    work_mode,
    experience_level,
    is_active,
    created_at,
    updated_at
FROM
    internview_schema.vacancies
WHERE
    id = $1;

