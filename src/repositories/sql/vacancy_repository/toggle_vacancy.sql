-- Update rows in 'internview_schema.vacancies' where condition is met
UPDATE
    internview_schema.vacancies
SET
    is_active = NOT is_active,
    updated_at = NOW()
WHERE
    id = $1
    AND recruiter_id = $2
RETURNING
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
    updated_at;

