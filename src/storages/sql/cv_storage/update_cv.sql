-- Update rows in 'internview_schema.cvs' where condition is met
UPDATE
    internview_schema.cvs
SET
    title = $3,
    description = $4,
    cv_pdf = $5,
    updated_at = NOW()
WHERE
    id = $1
    AND NOT EXISTS (
        SELECT
            1
        FROM
            internview_schema.cvs
        WHERE
            user_id = $2
            AND title = $3
            AND id != $1)
RETURNING
    updated_at
