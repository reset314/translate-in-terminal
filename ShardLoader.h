// ShardLoader.h
#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <cstdint>
#include <fstream>
#include <algorithm>
#include <cstring>

class ShardLoader {
public:
    // 单个词语
    struct Result {
        std::string_view word;
        std::string_view phonetic;
        std::string_view definition;
        std::string_view translate;
        std::string_view pos;
        std::string_view exchange;
    };

    // 加载指定分片
    bool load(const std::string& filepath);

    // 搜索
    std::vector<Result> search(const std::string& query, int max_dist = 2);

private:
    // 索引条目
    struct IndexEntry {
        uint32_t word_off;      uint16_t word_len;
        uint32_t phonetic_off;  uint16_t phonetic_len;
        uint32_t definition_off;uint16_t definition_len;
        uint32_t translate_off; uint16_t translate_len;
        uint32_t pos_off;       uint16_t pos_len;
        uint32_t exchange_off;  uint16_t exchange_len;
    };

    std::vector<char> data_;
    const char* word_pool_ = nullptr;
    const char* phonetic_pool_ = nullptr;
    const char* definition_pool_ = nullptr;
    const char* translate_pool_ = nullptr;
    const char* pos_pool_ = nullptr;
    const char* exchange_pool_ = nullptr;
    std::vector<IndexEntry> entries_;

    int levenshtein_limited(const std::string& s1, std::string_view s2, int limit);
};