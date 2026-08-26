-- Insert data into 'refresh_tokens'
INSERT INTO internview_schema.refresh_tokens(id, user_id, token_hash, expires_at)
    VALUES ($1, $2, $3, $4);

