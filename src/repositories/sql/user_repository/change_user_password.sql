-- Update rows in 'internview_schema.users' where condition is met
UPDATE
    internview_schema.users
SET
    password_hash = $2,
    password_version = password_version + 1
WHERE
    id = $1
RETURNING
    id
