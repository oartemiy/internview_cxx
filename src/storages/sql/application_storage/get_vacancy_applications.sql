-- Select all rows from 'internview_schema.applications'
SELECT
    *
FROM
    internview_schema.applications
WHERE
    vacancy_id = $1;

