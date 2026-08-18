INSERT INTO internview_schema.vacancies(id, recruiter_id, title, description, requirements, salary_range, location, work_mode, experience_level)
    VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)
ON CONFLICT
    DO NOTHING
RETURNING
    *;

