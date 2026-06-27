#include "AiService.h"
#include <json/json.h>

void AiService::getStandardAiReply(const std::string& prompt, 
                                   std::function<void(bool success, const std::string& cleanReply)>&& callback) 
{
    aiRepository_.fetchStandardChat(prompt, [callback = std::move(callback)](bool success, const std::string& rawPayload) {
        if (!success) {
            callback(false, "Failed to fetch response from AI Engine");
            return;
        }

        Json::Value root;
        Json::Reader reader;
        if (reader.parse(rawPayload, root) && root["choices"].size() > 0) {
            std::string cleanText = root["choices"][0]["message"]["content"].asString();
            callback(true, cleanText);
        } else {
            callback(false, "Parser error on AI payload");
        }
    });
}

void AiService::getStreamAiReply(const std::string& prompt,
                                 std::function<void(const std::string& cleanChunk, bool isEnd)>&& dataCallback) 
{
    aiRepository_.fetchStreamChat(prompt, [dataCallback = std::move(dataCallback)](const char* data, size_t size) {
        std::string rawData(data, size);
        
        // ניקוי המבנה הפרוטוקולי של OpenAI Server-Sent Events (data: {...})
        size_t pos = rawData.find("data: ");
        if (pos != std::string::npos) {
            std::string jsonStr = rawData.substr(pos + 6);
            if (jsonStr.find("[DONE]") != std::string::npos) {
                dataCallback("", true);
                return;
            }
            
            Json::Value root;
            Json::Reader reader;
            if (reader.parse(jsonStr, root)) {
                if (root["choices"].size() > 0 && root["choices"][0]["delta"].isMember("content")) {
                    std::string cleanWord = root["choices"][0]["delta"]["content"].asString();
                    dataCallback(cleanWord, false);
                }
            }
        }
    });
}
