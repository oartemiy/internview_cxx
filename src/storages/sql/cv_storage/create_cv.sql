-- Insert data into 'internview_schema.cv'
INSERT INTO internview_schema.cv(id, user_id, title, description, cv_pdf)
    VALUES ($1, $2, $3, $4, $5)
RETURNING
    created_at
