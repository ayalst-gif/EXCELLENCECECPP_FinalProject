#include <drogon/drogon.h>
#include <json/json.h>
#include <fstream>
#include <iostream>
int main() {
    
    std::string configPath = "./../config.json";
    // 1. קריאת קובץ ה-JSON באופן עצמאי כדי להציג חיווי מפורט
    std::ifstream configFile(configPath);
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;

    std::cout << "[INFO] Attempting to parse JSON from: " << configPath << std::endl;

    if (!Json::parseFromStream(builder, configFile, &root, &errs)) {
        std::cout << "\n=========================================" << std::endl;
        std::cout << "[ERROR] Failed to parse JSON file!" << std::endl;
        std::cout << "Error details: " << errs << std::endl;
        std::cout << "=========================================\n" << std::endl;
        return 1;
    }

    std::cout << "\n=========================================" << std::endl;
    std::cout << "[SUCCESS] !!! JSON PARSED SUCCESSFULLY !!!" << std::endl;

    // 2. שליפת הנתונים מתוך מערך ה-listeners הראשי
    if (root.isMember("listeners") && root["listeners"].isArray() && !root["listeners"].empty()) {
        // לוקחים את ה-listener הראשון (זה עם פורט 8585 בקובץ שלך)
        auto firstListener = root["listeners"][0];
        
        std::string address = firstListener.get("address", "0.0.0.0").asString();
        int port = firstListener.get("port", 0).asInt();

        std::cout << "Found Listener [0] in JSON:" << std::endl;
        std::cout << "  -> Target Address: " << address << std::endl;
        std::cout << "  -> Target Port:    " << port << std::endl;
    } else {
        std::cout << "[WARNING] Could not find a valid 'listeners' array in the root of JSON!" << std::endl;
    }
    std::cout << "=================================\n" << std::endl;

    // 3. העברת הקובץ התקין ל-Drogon והרצת השרת
    drogon::app().loadConfigFile(configPath);
    drogon::app().run();

    return 0;
}
