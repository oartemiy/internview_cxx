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
    is_active = TRUE
    AND ($3 IS NULL
        OR search_vector @@ plainto_tsquery('russian', $3)
        OR search_vector @@ plainto_tsquery('english', $3))
ORDER BY
    created_at DESC
LIMIT $1 OFFSET $2;

