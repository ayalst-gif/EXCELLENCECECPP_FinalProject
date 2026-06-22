#include "DB_Repository.h"
#include <stdexcept>

// מימוש הבנאי הפרטי - זורק Exception במקרה של כשל בחיבור
DB_Repository::DB_Repository(const std::string& dbClientName) {
    dbClient = drogon::app().getDbClient(dbClientName);
    if (!dbClient) {
        throw std::runtime_error("Database Connection Failed: Could not find DB client named '" + dbClientName + "'");
    }
}

// מימוש ה-getInstance: מבטיח מופע יחיד ובטוח לשימוש בריבוי תהליכונים (Thread-safe)
DB_Repository& DB_Repository::getInstance() {
    static DB_Repository instance; // נוצר פעם אחת בלבד בריצה הראשונה
    return instance;
}

// הרצת שאילתת SELECT חופשית
void DB_Repository::run_query(
    const std::string& sqlQuery,
    DbSelectCallback&& successCallback,
    DbErrorCallback&& errorCallback
) {
    dbClient->execSqlAsync(
        sqlQuery,
        [successCallback = std::move(successCallback)](const drogon::orm::Result& result) {
            successCallback(result);
        },
        [errorCallback = std::move(errorCallback)](const drogon::orm::DrogonDbException& e) {
            errorCallback(e.base().what());
        }
    );
}

// הרצת שאילתת שינוי (UPDATE/DELETE/INSERT)
void DB_Repository::run_update_query(
    const std::string& sqlQuery,
    DbUpdateCallback&& successCallback,
    DbErrorCallback&& errorCallback
) {
    dbClient->execSqlAsync(
        sqlQuery,
        [successCallback = std::move(successCallback)](const drogon::orm::Result& result) {
            successCallback(result.affectedRows());
        },
        [errorCallback = std::move(errorCallback)](const drogon::orm::DrogonDbException& e) {
            errorCallback(e.base().what());
        }
    );
}

// מימוש הפונקציה החכמה - בונה שאילתה ומריצה אותה
void DB_Repository::runSelectQuery(
    const std::string& sourceTable,
    DbSelectCallback&& successCallback,
    DbErrorCallback&& errorCallback,
    const std::string& selectFields,
    const std::string& condition
) {
    // בניית ה-SQL בצורה דינמית
    std::string sqlQuery = "SELECT " + selectFields + " FROM " + sourceTable;
    
    if (!condition.empty()) {
        sqlQuery += " WHERE " + condition;
    }

    // קריאה לפונקציית ההרצה הקיימת
    run_query(sqlQuery, std::move(successCallback), std::move(errorCallback));
}
