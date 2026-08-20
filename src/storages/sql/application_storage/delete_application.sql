-- Delete rows from 'internview_schema.applications' where condition is met
DELETE FROM internview_schema.applications
WHERE id = $1
    AND intern_id = $2
RETURNING
    id;

