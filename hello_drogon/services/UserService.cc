#include "UserService.h"
#include "models/Users.h"
#include "models/Roles.h"

// 1. שליפת רשימת המשתמשים הגולמית דרך ה-Repository
void UserService::getUsersList(GetUsersCallback&& callback) {
    userRepo.getAllUsers(
        [callback](const drogon::orm::Result& result) {
            callback(true, result, "");
        },
        [callback](const std::string& errorMsg) {
            callback(false, std::nullopt, errorMsg);
        }
    );
}

// 2. יצירת משתמש חדש דרך ה-Repository
void UserService::createNewUser(const std::string& username, const std::string& email, ActionCallback&& callback) {
    userRepo.insertUser(
        username,
        email,
        [callback](uint64_t affectedRows) {
            callback(true, "User created successfully. Affected rows: " + std::to_string(affectedRows));
        },
        [callback](const std::string& errorMsg) {
            callback(false, errorMsg);
        }
    );
}


void UserService::getUsersWithRolesList(GetOMRUsersCallback&& callback) {
    auto dbClient = drogon::app().getDbClient();
    
    // 🔥 התיקון: שינוי מ- r.id ל- r.role_id
    std::string sql = "SELECT u.id, u.username, u.email, COALESCE(r.role_name, 'No Role') as role_name "
                      "FROM users u "
                      "LEFT JOIN roles r ON u.role_id = r.role_id";

    auto sharedCallback = std::make_shared<GetOMRUsersCallback>(std::move(callback));

    dbClient->execSqlAsync(sql,
        [sharedCallback](const drogon::orm::Result& result) {
            std::vector<UserWithRoleDTO> dtos;
            dtos.reserve(result.size());

            for (const auto& row : result) {
                dtos.push_back({
                    row["id"].as<int32_t>(),
                    row["username"].as<std::string>(),
                    row["email"].as<std::string>(),
                    row["role_name"].as<std::string>()
                });
            }
            
            if (*sharedCallback) {
                (*sharedCallback)(true, dtos, "");
            }
        },
        [sharedCallback](const drogon::orm::DrogonDbException& e) {
            if (*sharedCallback) {
                (*sharedCallback)(false, {}, e.base().what());
            }
        }
    );
}

void UserService::deleteUser(int32_t userId, DeleteUserCallback&& callback) {
    // Get the global asynchronous database client managed by Drogon
    auto dbClient = drogon::app().getDbClient();
    
    // Wrap the callback in a shared_ptr to prevent memory corruption or 
    // std::bad_function_call if the Event Loop manages multiple lambdas.
    auto sharedCallback = std::make_shared<DeleteUserCallback>(std::move(callback));

    // Execute the SQL DELETE statement asynchronously to keep the server non-blocking
    dbClient->execSqlAsync(
        "DELETE FROM users WHERE id = $1",
        [sharedCallback](const drogon::orm::Result& r) {
            // Check if the shared pointer still contains a valid callback function
            if (*sharedCallback) {
                // Invoke the callback signaling success (true) with no error message
                (*sharedCallback)(true, "");
            }
        },
        [sharedCallback](const drogon::orm::DrogonDbException& e) {
            // Check if the shared pointer still contains a valid callback function
            if (*sharedCallback) {
                // Invoke the callback signaling failure (false) and forward the database error
                (*sharedCallback)(false, e.base().what());
            }
        },
        userId // Pass the integer variable safely to bind it to the $1 placeholder
    );
}

