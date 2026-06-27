#pragma once
#include "OpenAiRepository.h"
#include <functional>
#include <string>

class AiService {
public:
    void getStandardAiReply(const std::string& prompt, 
                            std::function<void(bool success, const std::string& cleanReply)>&& callback);

    void getStreamAiReply(const std::string& prompt,
                          std::function<void(const std::string& cleanChunk, bool isEnd)>&& dataCallback);

private:
    OpenAiRepository aiRepository_;
};
