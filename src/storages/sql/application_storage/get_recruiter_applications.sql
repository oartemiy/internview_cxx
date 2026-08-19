-- Select all rows from 'internview_schema.applications'
SELECT
    *
FROM
    internview_schema.applications
WHERE
    vacancy_id IN (
        SELECT
            id
        FROM
            internview_schema.vacancies
        WHERE
            recruiter_id = $1);

