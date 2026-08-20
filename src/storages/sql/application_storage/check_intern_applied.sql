SELECT
    1
FROM
    internview_schema.applications
WHERE
    intern_id = $1
    AND vacancy_id IN (
        SELECT
            id
        FROM
            internview_schema.vacancies
        WHERE
            recruiter_id = $2);

