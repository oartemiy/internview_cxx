-- Delete rows from 'internview_schema.users' where condition is met
DELETE FROM internview_schema.users
WHERE id = $1
RETURNING
    id
