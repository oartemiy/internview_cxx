-- Select all rows from 'internview_schema.vacancies'
SELECT
    *
FROM
    internview_schema.vacancies
WHERE
    is_active = TRUE
ORDER BY
    created_at DESC
LIMIT $1 OFFSET $2;

