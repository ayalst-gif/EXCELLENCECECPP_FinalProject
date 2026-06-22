#include "UsersController.h"

// 1. GET Route: Displays the web page with the table and the form
void UsersController::getUsers(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    userService.getUsersList([callback](bool success, const std::optional<drogon::orm::Result>& result, const std::string& errorMsg) {
        
        if (!success || !result.has_value()) {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k500InternalServerError);
            resp->setBody("Server Error: " + errorMsg);
            callback(resp);
            return;
        }

        // 1. Start building the HTML document (LTR for English)
        std::string html = "<!DOCTYPE html><html lang='en' dir='ltr'><head><meta charset='UTF-8'><title>User Management</title>";
        html += "<style>";
        html += "body { font-family: Arial, sans-serif; margin: 40px; background-color: #f4f7f6; text-align: left; }";
        html += "table { width: 100%; border-collapse: collapse; margin-top: 20px; background: white; }";
        html += "th, td { border: 1px solid #ddd; padding: 12px; text-align: left; }";
        html += "th { background-color: #007bff; color: white; }";
        html += "tr:nth-child(even) { background-color: #f9f9f9; }";
        html += ".form-container { background: white; padding: 20px; border-radius: 5px; box-shadow: 0 0 10px rgba(0,0,0,0.1); max-width: 400px; }";
        html += "input[type='text'], input[type='email'] { width: 100%; padding: 8px; margin: 5px 0 15px 0; box-sizing: border-box; border: 1px solid #ccc; border-radius: 4px; }";
        html += "</style></head><body>";

        html += "<h1>System User Management</h1>";
        
        // 2. Add New User Form
        html += "<div class='form-container'><h2>Add New User</h2>";
        html += "<form action='/users/add' method='POST'>";
        html += "<div><label>Username:</label><input type='text' name='username' required></div>";
        html += "<div><label>Email:</label><input type='email' name='email' required></div>";
        html += "<button type='submit' style='background:#28a745;color:white;padding:10px 15px;border:none;border-radius:4px;cursor:pointer;font-size:16px;'>Save User</button>";
        html += "</form></div><hr style='margin:40px 0; border:0; border-top:1px solid #ccc;'>";

        // 3. Build Users Table from DB
        html += "<h2>Existing Users List</h2>";
        html += "<table><thead><tr><th>ID</th><th>Username</th><th>Email</th></tr></thead><tbody>";
        
        for (const auto& row : result.value()) {
            html += "<tr>";
            html += "<td>" + row["id"].as<std::string>() + "</td>";
            html += "<td>" + row["username"].as<std::string>() + "</td>";
            html += "<td>" + row["email"].as<std::string>() + "</td>";
            html += "</tr>";
        }
        
        html += "</tbody></table></body></html>";

        // 4. Return the HTML response to the browser
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k200OK);
        resp->setContentTypeCode(drogon::CT_TEXT_HTML);
        resp->setBody(html);
        
        callback(resp);
    });
}

// 2. POST Route: Receives form data from browser and saves to DB
void UsersController::addUser(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    auto params = req->getParameters();
    std::string username = params["username"];
    std::string email = params["email"];

    userService.createNewUser(username, email, [callback](bool success, const std::string& message) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k303SeeOther);
        resp->addHeader("Location", "/users");
        callback(resp);
    });
}
