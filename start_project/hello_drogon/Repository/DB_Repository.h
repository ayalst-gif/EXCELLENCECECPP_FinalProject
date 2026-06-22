#pragma once
#include <string>
#include <functional>
#include <memory>
#include <drogon/drogon.h>

// הגדרות הטיפוסים הכלליות של הפרויקט
using DbSelectCallback = std::function<void(const drogon::orm::Result&)>;
using DbUpdateCallback = std::function<void(size_t rowsAffected)>;
using DbErrorCallback  = std::function<void(const std::string& errorMsg)>;

class DB_Repository {
private:
    drogon::orm::DbClientPtr dbClient;

    // בנאי פרטי כדי למנוע יצירת מופעים מבחוץ (חלק מחוקי הסינגלטון)
    DB_Repository(const std::string& dbClientName = "default");

public:
    // מחיקת אופרטורים של העתקה כדי למנוע שכפול של הסינגלטון
    DB_Repository(const DB_Repository&) = delete;
    DB_Repository& operator=(const DB_Repository&) = delete;

    // פונקציה סטטית לקבלת המופע היחיד במערכת (Meyers Singleton)
    static DB_Repository& getInstance();

    // 1. פונקציית תשתית להרצת שאילתות שליפה חופשיות (SELECT)
    void run_query(
        const std::string& sqlQuery,
        DbSelectCallback&& successCallback,
        DbErrorCallback&& errorCallback
    );

    // 2. פונקציית תשתית להרצת שאילתות שינוי (UPDATE/DELETE/INSERT)
    void run_update_query(
        const std::string& sqlQuery,
        DbUpdateCallback&& successCallback,
        DbErrorCallback&& errorCallback
    );

    // 3. הפונקציה החדשה לבנייה והרצה אוטומטית של שאילתות SELECT
    void runSelectQuery(
        const std::string& sourceTable,
        DbSelectCallback&& successCallback,
        DbErrorCallback&& errorCallback,
        const std::string& selectFields = "*",
        const std::string& condition = ""
    );
};
