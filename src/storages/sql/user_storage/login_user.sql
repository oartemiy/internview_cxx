-- Select all rows from 'internview_schema.users'
SELECT
    *
FROM
    internview_schema.users
WHERE
    login = $1
