-- Select all rows from 'internview_schema.cvs'
SELECT
    *
FROM
    internview_schema.cvs
WHERE
    user_id = $1;

