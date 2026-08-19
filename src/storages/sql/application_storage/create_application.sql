-- Insert data into 'internview_schema.applications'
INSERT INTO internview_schema.applications(id, vacancy_id, intern_id, cv_id, cover_letter)
    VALUES ($1, $2, $3, $4, $5)
RETURNING
    *;

