SELECT
    intern_id
FROM
    internview_schema.applications
WHERE
    cv_id = $1
    AND vacancy_id IN (
        SELECT
            id
        FROM
            internview_schema.vacancies
        WHERE
            recruiter_id = $2)
LIMIT 1;

