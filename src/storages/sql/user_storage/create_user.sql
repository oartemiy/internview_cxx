-- Insert data into 'internview_schema.users'
INSERT INTO internview_schema.users(id, login, password_hash, name, role, description, profile_pic)
    VALUES ($1, $2, $3, $4, $5, $6, $7)
ON CONFLICT (login)
    DO NOTHING
RETURNING
    created_at
