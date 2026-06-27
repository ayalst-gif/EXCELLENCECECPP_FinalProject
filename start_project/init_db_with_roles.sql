-- 1. מחיקת הטבלאות אם הן כבר קיימת (מחיקה בסדר נכון בגלל ה-Foreign Key)
DROP TABLE IF EXISTS users CASCADE;
DROP TABLE IF EXISTS roles CASCADE;

-- 2. יצירת טבלת התפקידים (roles)
CREATE TABLE roles (
    role_id SERIAL PRIMARY KEY,
    role_name VARCHAR(50) UNIQUE NOT NULL
);

-- 3. יצירת טבלת המשתמשים (users) המקושרת לטבלת התפקידים
CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    email VARCHAR(100) NOT NULL,
    role_id INT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    
    CONSTRAINT fk_user_role FOREIGN KEY (role_id) REFERENCES roles(role_id) ON DELETE RESTRICT
);

-- 4. הכנסת תפקידים בסיסיים למערכת
INSERT INTO roles (role_name) VALUES 
('Admin'),  -- יקבל מפתח 1
('User'),   -- יקבל מפתח 2
('Guest');  -- יקבל מפתח 3

-- 5. הכנסת כמות מורחבת של משתמשים לבדיקות (Data Seeding)
INSERT INTO users (username, email, role_id) VALUES 
('ayal_admin', 'ayal@example.com', 1),
('test_user', 'user@example.com', 2),
('daniela_dev', 'daniela@example.com', 1),
('john_doe', 'john.doe@example.com', 2),
('jane_smith', 'jane.smith@example.com', 2),
('guest_shopper', 'guest1@example.com', 3),
('alex_green', 'alex.g@example.com', 2),
('system_monitor', 'monitor@example.com', 3);
