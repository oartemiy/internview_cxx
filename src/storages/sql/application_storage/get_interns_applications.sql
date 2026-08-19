-- Select all rows from 'internview_schema.applications'
SELECT * FROM internview_schema.applications
WHERE intern_id = $1;