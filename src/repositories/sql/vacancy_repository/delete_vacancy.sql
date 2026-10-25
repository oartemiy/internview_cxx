DELETE FROM internview_schema.vacancies
WHERE id = $1
    AND recruiter_id = $2
RETURNING
    id
