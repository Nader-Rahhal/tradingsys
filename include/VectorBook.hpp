#pragma once
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cstdint>

template <typename Compare>
class VectorBook {
    std::vector<std::pair<int64_t, int64_t>> levels_;
    Compare cmp_;

    auto find_level(int64_t price) {
        return std::find_if(levels_.begin(), levels_.end(),
            [price](const auto& p) { return p.first == price; });
    }

    auto find_level(int64_t price) const {
        return std::find_if(levels_.begin(), levels_.end(),
            [price](const auto& p) { return p.first == price; });
    }

public:
    void addOrder(int64_t price, int64_t volume) {
        auto it = find_level(price);
        if (it != levels_.end()) {
            it->second += volume;
        } 
        else {
            auto pos = std::find_if(levels_.begin(), levels_.end(),
                [&](const auto& p) { return cmp_(price, p.first); });
            levels_.insert(pos, {price, volume});
        }
    }

    void deleteOrder(int64_t price, int64_t volume) {
        auto it = find_level(price);
        if (it == levels_.end())
            throw std::invalid_argument("price level not found");
        it->second -= volume;
        if (it->second <= 0)
            levels_.erase(it);
    }

    void modifyOrder(int64_t oldPrice, int64_t oldVolume, int64_t newPrice, int64_t newVolume) {
        deleteOrder(oldPrice, oldVolume);
        addOrder(newPrice, newVolume);
    }

    int64_t volumeAt(int64_t price) const {
        auto it = find_level(price);
        return it == levels_.end() ? 0 : it->second;
    }

    int64_t bestPrice() const {
        if (levels_.empty())
            throw std::runtime_error("book is empty");
        return levels_.front().first;
    }

    bool empty() const {
        return levels_.empty();
    }
};