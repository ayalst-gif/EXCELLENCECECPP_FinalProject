#pragma once
#include <functional>
#include <string>
#include <optional> 
#include <drogon/drogon.h>
#include <drogon/orm/Mapper.h>
#include <vector>

// הכלת ה-Repository והמודלים
#include "UserRepository.h" 
#include "../models/Users.h"
#include "../models/Roles.h"

// הגדרת מבנה נתונים שטוח להעברת המידע ל-Controller (Data Transfer Object)
struct UserWithRoleDTO {
    int32_t id;
    std::string username;
    std::string email;
    std::string roleName;
};

using GetUsersCallback = std::function<void(bool success, const std::optional<drogon::orm::Result>& result, const std::string& errorMsg)>;
using ActionCallback   = std::function<void(bool success, const std::string& message)>;
// תוסיף את זה לצד ה-using-ים האחרים ב-UserService.h
using DeleteUserCallback = std::function<void(bool success, const std::string& errorMsg)>;

// עדכון ה-Callback שיעביר את וקטור ה-DTOs במקום המודל הגולמי של ה-ORM
using GetOMRUsersCallback = std::function<void(bool success, const std::vector<UserWithRoleDTO>& users, const std::string& errorMsg)>;

class UserService {
private:
    UserRepository userRepo;

public:
    UserService() = default;
    ~UserService() = default;

    void getUsersList(GetUsersCallback&& callback);
    void createNewUser(const std::string& username, const std::string& email, ActionCallback&& callback);
    void getUsersWithRolesList(GetOMRUsersCallback&& callback);
    
    // בתוך ה-class UserService (תחת public):
    void deleteUser(int32_t userId, DeleteUserCallback&& callback);
};
