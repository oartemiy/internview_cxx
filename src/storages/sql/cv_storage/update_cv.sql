-- Update rows in 'internview_schema.cvs' where condition is met
UPDATE
    internview_schema.cvs
SET
    title = $2,
    description = $3,
    cv_pdf = $4,
    updated_at = NOW()
WHERE
    id = $1
RETURNING
    updated_at
