-- Delete rows from 'internview_schema.refresh_tokens' where condition is met
DELETE FROM internview_schema.refresh_tokens
WHERE id = $1;

