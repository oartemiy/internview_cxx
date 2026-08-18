-- Select all rows from 'internview_schema.vacancies'
SELECT
    *
FROM
    internview_schema.vacancies
WHERE
    recruiter_id = $1;

