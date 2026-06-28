#include "UserWithRolesController.h"

// הכלת השירות
#include "./../services/UserService.h" 

void UserWithRolesController::getUsersWithRoles(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    UserService userService;

    userService.getUsersWithRolesList([callback](bool success, const std::vector<UserWithRoleDTO>& users, const std::string& errorMsg) {
        auto resp = drogon::HttpResponse::newHttpResponse();

        if (!success) {
            resp->setStatusCode(drogon::k500InternalServerError);
            resp->setBody("Database Error via Service: " + errorMsg);
            callback(resp);
            return;
        }

        std::string html = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><title>Clean Architecture</title>";
        html += "<style>body { font-family: Arial, sans-serif; margin: 40px; background-color: #f4f7f6; }";
        html += "table { width: 100%; border-collapse: collapse; margin-top: 20px; background: white; }";
        html += "th, td { border: 1px solid #ddd; padding: 12px; text-align: left; }";
        html += "th { background-color: #2a5298; color: white; }";
        // עיצוב קטן לכפתור המחיקה
        html += ".btn-delete { background-color: #e74c3c; color: white; border: none; padding: 6px 12px; cursor: pointer; border-radius: 4px; }";
        html += ".btn-delete:hover { background-color: #c0392b; }</style></head><body>";

        html += "<h1>Users & Roles (Fetched via UserService Layer)</h1>";
        // 🔥 הוספת כותרת עמודה "Actions" ב-thead
        html += "<table><thead><tr><th>ID</th><th>Username</th><th>Email</th><th>Role Name</th><th>Actions</th></tr></thead><tbody>";
        
        for (const auto& user : users) {
            html += "<tr>";
            html += "<td>" + std::to_string(user.id) + "</td>";
            html += "<td>" + user.username + "</td>";
            html += "<td>" + user.email + "</td>";
            html += "<td>" + user.roleName + "</td>";
            
            // 🔥 הוספת כפתור מחיקה עטוף בטופס (Form) ששולח את ה-ID ב-POST
            html += "<td>";
            html += "<form method='POST' action='/users_roles/delete' style='margin:0;'>";
            html += "<input type='hidden' name='id' value='" + std::to_string(user.id) + "'>";
            html += "<button type='submit' class='btn-delete' onclick=\"return confirm('Are you sure?');\">Remove</button>";
            html += "</form>";
            html += "</td>";
            
            html += "</tr>";
        }
        
        html += "</tbody></table></body></html>";

        resp->setStatusCode(drogon::k200OK);
        resp->setContentTypeCode(drogon::CT_TEXT_HTML);
        resp->setBody(html);
        callback(resp);
    });
}

void UserWithRolesController::deleteUser(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    auto idStr = req->getParameter("id");
    auto resp = drogon::HttpResponse::newHttpResponse();
    
    // הגדרת הפניה מחדש (Redirect) כברירת מחדל
    resp->setStatusCode(drogon::k303SeeOther);
    resp->addHeader("Location", "/users_roles");

    if (idStr.empty()) {
        callback(resp);
        return;
    }

    // המרה של ה-ID משרשרת למספר
    int32_t userId = std::stoi(idStr);

    // 🔥 התיקון: קריאה לשירות במקום ישירות ל-DB
    UserService userService;
    userService.deleteUser(userId, [callback, resp](bool success, const std::string& errorMsg) {
    if (!success) {
            LOG_ERROR << "Failed to delete user via service: " << errorMsg;
            // גם אם נכשל, נחזיר את המשתמש לטבלה (או שתוכל להציג לו שגיאה לבחירתך)
        }
        callback(resp);
    });
}

void UserWithRolesController::addUserWithORM(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k303SeeOther);
    resp->addHeader("Location", "/users_roles");
    callback(resp);
}
