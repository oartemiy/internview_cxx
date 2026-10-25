-- Insert data into 'internview_schema.cvs'
INSERT INTO internview_schema.cvs(id, user_id, title, description, cv_pdf)
    VALUES ($1, $2, $3, $4, $5)
RETURNING
    created_at
