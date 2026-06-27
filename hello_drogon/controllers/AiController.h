#pragma once
#include <drogon/HttpController.h>
#include "../services/AiService.h" // ודא שהנתיב ל-Service נכון לפי מבנה התיקיות שלך

class AiController : public drogon::HttpController<AiController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AiController::askAiStandard, "/api/chat/blocking", drogon::Post);
    ADD_METHOD_TO(AiController::askAiStream, "/api/chat/stream", drogon::Post);
    METHOD_LIST_END

    void askAiStandard(const drogon::HttpRequestPtr& req, 
                       std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void askAiStream(const drogon::HttpRequestPtr& req, 
                     std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    AiService aiService_;
};

