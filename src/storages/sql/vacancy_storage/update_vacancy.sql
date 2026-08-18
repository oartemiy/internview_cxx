-- Update rows in 'internview_schema.vacancies' where condition is met
UPDATE
    internview_schema.vacancies
SET
    title = $2,
    description = $3,
    requirements = $4,
    salary_range = $5,
    location = $6,
    work_mode = $7,
    experience_level = $8,
    updated_at = NOW()
WHERE
    id = $1
RETURNING
    id;

