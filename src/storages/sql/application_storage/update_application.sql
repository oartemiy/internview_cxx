-- Update rows in 'internview_schema.applications' where condition is met
UPDATE
    internview_schema.applications
SET
    cv_id = $2,
    status = $3,
    cover_letter = $4
WHERE
    id = $1
RETURNING
    *;

