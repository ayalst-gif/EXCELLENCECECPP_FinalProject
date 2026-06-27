#pragma once
#include "DB_Repository.h"

class UserRepository {
public:
    UserRepository() = default;

    void getAllUsers(DbSelectCallback&& success, DbErrorCallback&& error);

    // הפונקציה החדשה להוספת משתמש
    void insertUser(
        const std::string& username,
        const std::string& email,
        DbUpdateCallback&& success,
        DbErrorCallback&& error
    );
};

