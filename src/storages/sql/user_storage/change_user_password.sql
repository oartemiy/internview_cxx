-- Update rows in 'internview_schema.user' where condition is met
UPDATE
    internview_schema.user
SET
    password_hash = $2
WHERE
    id = $1
RETURNING
    id
