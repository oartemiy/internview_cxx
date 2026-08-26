-- Select all rows from 'internview_schema.refresh_tokens'
SELECT
    *
FROM
    internview_schema.refresh_tokens
WHERE
    token_hash = $1;

