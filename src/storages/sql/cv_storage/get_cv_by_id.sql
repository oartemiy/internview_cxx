-- Select all rows from 'internview_schema.cvs'
SELECT
    *
FROM
    internview_schema.cvs
WHERE
    id = $1
    AND user_id = $2;

