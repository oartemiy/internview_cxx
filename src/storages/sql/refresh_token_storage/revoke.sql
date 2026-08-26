-- Update rows in 'internview_schema.refresh_tokens' where condition is met
UPDATE
    internview_schema.refresh_tokens
SET
    revoked = TRUE
WHERE
    id = $1;

