#include "OpenAiRepository.h"
#include <json/json.h>
#include <iostream>
#include <string>
#include <functional>
#include <cstdio>
#include <fstream>
#include <memory>
#include <array>
#include <unistd.h>
#include "repo_key.h"

OpenAiRepository::OpenAiRepository() 
    : apiKey_(REPO_KEY) 
{
}

static std::string execCommand(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        return "";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void OpenAiRepository::fetchStandardChat(const std::string& prompt, 
                                         std::function<void(bool success, const std::string& rawPayload)>&& callback) 
{
    Json::Value body;
    Json::Value part;
    part["text"] = prompt;
    Json::Value content;
    content["parts"].append(part);
    body["contents"].append(content);
    
    std::string jsonStr = Json::FastWriter().write(body);
    std::string tempFilename = "/tmp/gemini_req_" + std::to_string(getpid()) + ".json";
    
    std::ofstream out(tempFilename);
    if (out.is_open()) {
        out << jsonStr;
        out.close();
    }

    std::string curlCmd = "curl -s -X POST \"https://generativelanguage.googleapis.com/v1/models/gemini-2.5-flash:generateContent?key=" + apiKey_ + "\" "
                          "-H \"Content-Type: application/json\" "
                          "-d @" + tempFilename;

    std::string responseBody = execCommand(curlCmd);
    std::remove(tempFilename.c_str());

    if (responseBody.empty()) {
        callback(false, "");
        return;
    }

    Json::Value root;
    Json::Reader reader;
    if (reader.parse(responseBody, root)) {
        try {
            std::string textOut = root["candidates"][0]["content"]["parts"][0]["text"].asString();
            Json::Value fakeOpenAi;
            fakeOpenAi["choices"][0]["message"]["content"] = textOut;
            callback(true, Json::FastWriter().write(fakeOpenAi));
            return;
        } catch (...) {}
    }
    callback(false, "");
}

// 🔥 פונקציית סטרימינג אמיתית בזמן אמת עוקפת באפרינג 🔥
void OpenAiRepository::fetchStreamChat(const std::string& prompt,
                                       std::function<void(const char* data, size_t size)>&& onChunkReceived) 
{
    Json::Value body;
    Json::Value part;
    part["text"] = prompt;
    Json::Value content;
    content["parts"].append(part);
    body["contents"].append(content);

    std::string jsonStr = Json::FastWriter().write(body);
    std::string tempFilename = "/tmp/gemini_stream_req_" + std::to_string(getpid()) + ".json";
    
    std::ofstream out(tempFilename);
    if (out.is_open()) {
        out << jsonStr;
        out.close();
    }

    // 🚀 שימוש ב-streamGenerateContent ודגל -N (No-Buffer) להזרמה מיידית
    std::string curlCmd = "curl -N -s -X POST \"https://generativelanguage.googleapis.com/v1/models/gemini-2.5-flash:streamGenerateContent?key=" + apiKey_ + "\" "
                          "-H \"Content-Type: application/json\" "
                          "-d @" + tempFilename;

    std::cout << "\n==================================================" << std::endl;
    std::cout << "[BYPASS DEBUG] STARTING REAL-TIME CURL STREAM..." << std::endl;
    std::cout << "==================================================\n" << std::endl;

    FILE* pipe = popen(curlCmd.c_str(), "r");
    if (!pipe) {
        std::remove(tempFilename.c_str());
        std::string doneSignal = "data: [DONE]\n\n";
        onChunkReceived(doneSignal.c_str(), doneSignal.length());
        return;
    }

    char buffer[4096];
    // קריאת הפלט שורה אחר שורה בזמן אמת כשהוא נפלט מגוגל
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::string line(buffer);
        
        // שליפה חכמה ומהירה של המילים מתוך הסטרים (מונע קריסות של חלקי JSON)
        size_t textPos = line.find("\"text\": \"");
        if (textPos != std::string::npos) {
            size_t start = textPos + 9;
            size_t end = line.find("\"", start);
            if (end != std::string::npos) {
                std::string extractedText = line.substr(start, end - start);
                
                // המרה למבנה SSE ש-AiService והדפדפן צריכים
                std::string simulatedSse = "data: {\"choices\":[{\"delta\":{\"content\":\"" + extractedText + "\"}}]}\n\n";
                
                std::cout << "[STREAMING CHUNK TO UI]: " << extractedText << std::endl;
                onChunkReceived(simulatedSse.c_str(), simulatedSse.length());
            }
        }
    }

    pclose(pipe);
    std::remove(tempFilename.c_str());

    // שליחת אות סיום רשמי לפרוטוקול ה-SSE
    std::cout << "\n==================================================" << std::endl;
    std::cout << "[BYPASS DEBUG] STREAM FINISHED SUCCESSFULLY" << std::endl;
    std::cout << "==================================================\n" << std::endl;

    std::string doneSignal = "data: [DONE]\n\n";
    onChunkReceived(doneSignal.c_str(), doneSignal.length());
}
