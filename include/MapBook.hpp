#pragma once
#include <map>
#include <stdexcept>
#include <cstdint>

template <typename Compare>
class MapBook {
    std::map<int64_t, int64_t, Compare> levels_;

    void applyDelta(int64_t price, int64_t delta) {
        auto [it, inserted] = levels_.try_emplace(price, delta);
        if (!inserted) {
            it->second += delta;
            if (it->second <= 0)
                levels_.erase(it);
        }
    }

public:
    void addOrder(int64_t price, int64_t volume) {
        applyDelta(price, volume);
    }

    void deleteOrder(int64_t price, int64_t volume) {
        auto it = levels_.find(price);
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
        auto it = levels_.find(price);
        return it == levels_.end() ? 0 : it->second;
    }

    int64_t bestPrice() const {
        if (levels_.empty())
            throw std::runtime_error("book is empty");
        return levels_.begin()->first;
    }

    bool empty() const { return levels_.empty(); }
};