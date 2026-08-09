-- Update rows in 'internview_schema.users' where condition is met
UPDATE
    internview_schema.users
SET
    login = $1,
    name = $2,
    description = $3,
    profile_pic = $4
WHERE
    id = $5 AND NOT EXISTS (
        SELECT 1 FROM internview_schema.users
        WHERE login = $1 AND id != $5
    )

RETURNING
    id

