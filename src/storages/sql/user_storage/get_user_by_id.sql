-- Select all rows from 'internview_schema.user'
SELECT
    *
FROM
    internview_schema.user
WHERE
    id = $1
