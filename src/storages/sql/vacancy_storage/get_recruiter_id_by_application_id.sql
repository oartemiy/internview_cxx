SELECT
    recruiter_id
FROM
    internview_schema.vacancies
WHERE
    id =(
        SELECT
            vacancy_id
        FROM
            internview_schema.applications
        WHERE
            id = $1);

