-- Delete rows from 'internview_schema.cvs' where condition is met
DELETE FROM internview_schema.cvs
WHERE id = $1
    AND user_id = $2
RETURNING
    id
