#include "AiController.h"
#include <json/json.h>
#include <utility>
#include <memory> // Required for std::shared_ptr

// 1. Standard Blocking Endpoint
void AiController::askAiStandard(const drogon::HttpRequestPtr& req, 
                                 std::function<void(const drogon::HttpResponsePtr&)>&& callback) 
{
    auto json = req->getJsonObject();
    std::string prompt = json ? (*json)["prompt"].asString() : "";

    aiService_.getStandardAiReply(prompt, [callback = std::move(callback)](bool success, const std::string& cleanReply) {
        Json::Value outputJson;
        auto resp = drogon::HttpResponse::newHttpResponse();
        
        if (!success) {
            outputJson["error"] = cleanReply;
            resp->setStatusCode(drogon::k500InternalServerError);
        } else {
            outputJson["reply"] = cleanReply;
            resp->setStatusCode(drogon::k200OK);
        }
        
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        resp->setBody(Json::FastWriter().write(outputJson));
        callback(resp);
    });
}

// 2. Streaming Endpoint (Fixed with std::shared_ptr conversion)
void AiController::askAiStream(const drogon::HttpRequestPtr& req, 
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback) 
{
    auto json = req->getJsonObject();
    std::string prompt = json ? (*json)["prompt"].asString() : "";

    auto resp = drogon::HttpResponse::newAsyncStreamResponse(
        [this, prompt](drogon::ResponseStreamPtr stream) {
            
            // CRITICAL FIX: Convert the unique_ptr to a shared_ptr.
            // This makes it safe to copy and bypasses the std::function copy-constructible constraint.
            std::shared_ptr<drogon::ResponseStream> sharedStream = std::move(stream);
            
            aiService_.getStreamAiReply(prompt, [sharedStream](const std::string& cleanChunk, bool isEnd) {
                if (isEnd) {
                    sharedStream->close(); 
                } else {
                    std::string sseMessage = "data: " + cleanChunk + "\n\n";
                    sharedStream->send(sseMessage);
                }
            });
        }
    );
    
    resp->setContentTypeString("text/event-stream");
    callback(resp);
}
