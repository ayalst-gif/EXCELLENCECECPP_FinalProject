#pragma once
#include <drogon/HttpController.h>
#include "UserService.h"

class UsersController : public drogon::HttpController<UsersController> {
private:
    UserService userService;

public:
    METHOD_LIST_BEGIN
    // נתיב GET לקבלת רשימה
    ADD_METHOD_TO(UsersController::getUsers, "/users", drogon::Get);
    // נתיב POST חדש ליצירת משתמש
    ADD_METHOD_TO(UsersController::addUser, "/users/add", drogon::Post);
    METHOD_LIST_END

    void getUsers(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    // מתודה חדשה לטיפול ב-POST
    void addUser(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );
};
