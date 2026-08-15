-- Delete rows from 'internview_schema.user' where condition is met
DELETE FROM internview_schema.user
WHERE id = $1
RETURNING
    id
