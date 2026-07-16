// ShardLoader.cpp
#include "ShardLoader.h"
#include <cassert>

bool ShardLoader::load(const std::string& filepath) {
    std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);
    if (!ifs.is_open())
        return false;

    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    if (size < 32)
        return false;

    data_.resize(size);
    if (!ifs.read(data_.data(), size))
        return false;

    const char* ptr = data_.data();

    // header
    uint32_t magic, entry_count;
    uint32_t word_off, phonetic_off, definition_off, translate_off, pos_off, exchange_off;
    std::memcpy(&magic,          ptr, 4); ptr += 4;
    std::memcpy(&entry_count,    ptr, 4); ptr += 4;
    std::memcpy(&word_off,       ptr, 4); ptr += 4;
    std::memcpy(&phonetic_off,   ptr, 4); ptr += 4;
    std::memcpy(&definition_off, ptr, 4); ptr += 4;
    std::memcpy(&translate_off,  ptr, 4); ptr += 4;
    std::memcpy(&pos_off,        ptr, 4); ptr += 4;
    std::memcpy(&exchange_off,   ptr, 4); ptr += 4;

    if (magic != 0x44494354)  // DICT
        return false;

    // entry_count_ = entry_count;
    word_pool_       = data_.data() + word_off;
    phonetic_pool_   = data_.data() + phonetic_off;
    definition_pool_ = data_.data() + definition_off;
    translate_pool_  = data_.data() + translate_off;
    pos_pool_        = data_.data() + pos_off;
    exchange_pool_   = data_.data() + exchange_off;

    // 索引区offset
    const size_t index_size = entry_count * 6 * 6;
    const char* index_start = data_.data() + size - index_size;

    // 解析
    entries_.resize(entry_count);
    for (size_t i = 0; i < entry_count; ++i) {
        auto& e = entries_[i];
        const char* p = index_start + i * (6 * 6);

        std::memcpy(&e.word_off,        p, 4); p += 4; std::memcpy(&e.word_len,        p, 2); p += 2;
        std::memcpy(&e.phonetic_off,    p, 4); p += 4; std::memcpy(&e.phonetic_len,    p, 2); p += 2;
        std::memcpy(&e.definition_off,  p, 4); p += 4; std::memcpy(&e.definition_len,  p, 2); p += 2;
        std::memcpy(&e.translate_off,   p, 4); p += 4; std::memcpy(&e.translate_len,   p, 2); p += 2;
        std::memcpy(&e.pos_off,         p, 4); p += 4; std::memcpy(&e.pos_len,         p, 2); p += 2;
        std::memcpy(&e.exchange_off,    p, 4); p += 4; std::memcpy(&e.exchange_len,    p, 2); p += 2;
    }

    return true;
}

std::vector<ShardLoader::Result> ShardLoader::search(const std::string& query, int max_dist) {
    std::vector<Result> results;
    const size_t query_len = query.size();

    std::vector<std::pair<double, Result>> results_v;
    results_v.reserve(entries_.size());

    for (const auto& idx : entries_) {
        // 剪枝
        if (std::abs(static_cast<int>(idx.word_len) - static_cast<int>(query_len)) > max_dist)
            continue;

        std::string_view word(data_.data() + idx.word_off, idx.word_len);
        int dist = levenshtein_limited(query, word, max_dist);

        if (dist <= max_dist) {
            Result r;
            r.word       = word;
            r.phonetic   = std::string_view(data_.data() + idx.phonetic_off,   idx.phonetic_len);
            r.definition = std::string_view(data_.data() + idx.definition_off, idx.definition_len);
            r.translate  = std::string_view(data_.data() + idx.translate_off,  idx.translate_len);
            r.pos        = std::string_view(data_.data() + idx.pos_off,        idx.pos_len);
            r.exchange   = std::string_view(data_.data() + idx.exchange_off,   idx.exchange_len);

            double score = static_cast<double>(query_len + idx.word_len - dist) /
                           (query_len + idx.word_len);
            results_v.emplace_back(score, std::move(r));
        }
    }

    if (results_v.empty()) return {};

    // 按分数从高到低排序
    std::sort(results_v.begin(), results_v.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    if (results_v[0].first == 1.00){
        results.push_back(std::move(results_v[0].second));
        return results;
    }

    results.reserve(results_v.size());
    for (auto& pair : results_v) {
        results.push_back(std::move(pair.second));
    }

    return results;
}

int ShardLoader::levenshtein_limited(const std::string& s1, std::string_view s2, int limit) {
    size_t n = s1.size(), m = s2.size();
    if (std::abs((int)n - (int)m) > limit) return limit + 1;
    if (n == 0) return (int)m <= limit ? (int)m : limit + 1;
    if (m == 0) return (int)n <= limit ? (int)n : limit + 1;

    std::string_view a = s1;
    std::string_view b = s2;
    if (n > m) {
        std::swap(a, b);
        std::swap(n, m);
    }
    std::vector<int> prev(m + 1), curr(m + 1);
    for (size_t j = 0; j <= m; ++j) prev[j] = j;

    for (size_t i = 1; i <= n; ++i) {
        curr[0] = i;
        int min_val = curr[0];
        for (size_t j = 1; j <= m; ++j) {
            int cost = (a[i-1] == b[j-1]) ? 0 : 1;
            curr[j] = std::min({curr[j-1] + 1, prev[j] + 1, prev[j-1] + cost});
            if (curr[j] < min_val) min_val = curr[j];
        }
        if (min_val > limit) return limit + 1; // 剪枝
        std::swap(prev, curr);
    }
    return prev[m];
}