DELETE FROM internview_schema.refresh_tokens
WHERE revoked = TRUE
    OR expires_at <= NOW();

