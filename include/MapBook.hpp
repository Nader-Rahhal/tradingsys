#pragma once  

#include <map>
#include <unordered_map>
#include <stdexcept>
#include <cstdint>

template <typename Compare>
class MapBook {

    std::map<int64_t, int64_t, Compare> levels_;

    struct OrderMeta {
        int64_t price;
        int64_t volume;
    };

    std::unordered_map<uint64_t, OrderMeta> orderIndex_;

public:
    void addOrder(uint64_t id, int64_t price, int64_t volume) {

        if (orderIndex_.count(id))
            throw std::invalid_argument("duplicate order id");

        orderIndex_[id] = { price, volume };
        levels_[price] += volume;
    }

    void deleteOrder(uint64_t id) {
        auto it = orderIndex_.find(id);

        if (it == orderIndex_.end())
            throw std::invalid_argument("order not found");

        auto& [price, volume] = it->second;
        levels_[price] -= volume;

        if (levels_[price] == 0)
            levels_.erase(price);
        
        orderIndex_.erase(it);
    }

    void modifyOrder(uint64_t id, int64_t newPrice, int64_t newVolume) {
        auto it = orderIndex_.find(id);

        if (it == orderIndex_.end())
            throw std::invalid_argument("order not found");

        auto& [oldPrice, oldVolume] = it->second;
        levels_[oldPrice] -= oldVolume;
        
        if (levels_[oldPrice] == 0)
            levels_.erase(oldPrice);

        it->second = { newPrice, newVolume };
        levels_[newPrice] += newVolume;
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