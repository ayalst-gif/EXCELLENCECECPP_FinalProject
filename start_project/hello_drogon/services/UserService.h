#pragma once
#include <functional>
#include <string>
#include <optional> // <-- להוסיף את זה עבור std::optional
#include <drogon/drogon.h>
#include "UserRepository.h"

// התיקון: עטיפת ה-Result ב-std::optional
using GetUsersCallback = std::function<void(bool success, const std::optional<drogon::orm::Result>& result, const std::string& errorMsg)>;
using ActionCallback   = std::function<void(bool success, const std::string& message)>;

class UserService {
private:
    UserRepository userRepo;

public:
    UserService() = default;
    ~UserService() = default;

    void getUsersList(GetUsersCallback&& callback);
    void createNewUser(const std::string& username, const std::string& email, ActionCallback&& callback);
};
