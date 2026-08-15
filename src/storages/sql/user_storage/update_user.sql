-- Update rows in 'internview_schema.user' where condition is met
UPDATE
    internview_schema.user
SET
    login = $1,
    name = $2,
    description = $3,
    profile_pic = $4
WHERE
    id = $5 AND NOT EXISTS (
        SELECT 1 FROM internview_schema.user
        WHERE login = $1 AND id != $5
    )

RETURNING
    id

