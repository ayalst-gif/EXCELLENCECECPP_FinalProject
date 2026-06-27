#pragma once
#include <drogon/HttpClient.h>
#include <functional>
#include <string>

class OpenAiRepository {
public:
    OpenAiRepository();
    
    // שליחת בקשה רגילה
    void fetchStandardChat(const std::string& prompt, 
                           std::function<void(bool success, const std::string& rawPayload)>&& callback);

    // שליחת בקשת סטרימינג
    void fetchStreamChat(const std::string& prompt,
                         std::function<void(const char* data, size_t size)>&& onChunkReceived);

private:
    drogon::HttpClientPtr client_;
    const std::string apiKey_ = "AIzaSyCjfFeAwLFNgahaaeL1Ah0Re0F9aagOEoQ";
};
