-- Update rows in 'internview_schema.vacancies' where condition is met
UPDATE
    internview_schema.vacancies
SET
    is_active = NOT is_active,
    updated_at = NOW()
WHERE
    id = $1 AND recruiter_id = $2
RETURNING
    *