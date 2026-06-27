#include "UserService.h"

// 1. מימוש שליפת רשימת המשתמשים
void UserService::getUsersList(GetUsersCallback&& callback) {
    
    userRepo.getAllUsers(
        [callback](const drogon::orm::Result& result) {
            // בהצלחה: עוטפים את ה-result בתוך std::optional
            callback(true, std::make_optional(result), "");
        },
        [callback](const std::string& dbError) {
            // בשגיאה: מחזירים std::nullopt במקום לנסות לבנות Result ריק
            callback(false, std::nullopt, "Failed to fetch users: " + dbError);
        }
    );
}

// 2. מימוש יצירת משתמש חדש
void UserService::createNewUser(
    const std::string& username,
    const std::string& email,
    ActionCallback&& callback
) {
    // ולידציה בסיסית
    if (username.empty() || email.empty()) {
        callback(false, "Username and Email cannot be empty.");
        return;
    }

    // פנייה ל-Repository להזרקה לדאטהבייס
    userRepo.insertUser(
        username,
        email,
        [callback](size_t rowsAffected) {
            if (rowsAffected > 0) {
                callback(true, "User created successfully!");
            } else {
                callback(false, "Failed to create user: No rows affected.");
            }
        },
        [callback](const std::string& dbError) {
            // טיפול בשגיאות מפתח ייחודי (UNIQUE constraint)
            if (dbError.find("duplicate key") != std::string::npos || dbError.find("UNIQUE") != std::string::npos) {
                callback(false, "Registration failed: Username already exists.");
            } else {
                callback(false, "Database error: " + dbError);
            }
        }
    );
}
