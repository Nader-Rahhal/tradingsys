#pragma once
#include <random>
#include <vector>

enum class Op { Add, Delete, Modify };

struct Event {
    Op      op;
    int64_t price;
    int64_t volume;
    // for Modify only: the old price/volume to remove
    int64_t old_price;
    int64_t old_volume;
};

struct WorkloadFixture {
    std::vector<Event>   events;
    std::vector<int64_t> seed_prices;
    std::vector<int64_t> seed_volumes;
};

inline WorkloadFixture make_workload(int n, uint32_t seed, int n_price_levels,
                                     int add_pct, int delete_pct) {
    std::mt19937 rng(seed);
    std::geometric_distribution<int> level_dist(0.3);

    auto sample_price = [&]() -> int64_t {
        int level = std::min(level_dist(rng), n_price_levels - 1);
        return 10000 + level * 100;
    };

    struct LiveOrder { int64_t price, volume; };
    std::vector<LiveOrder> live_orders;

    WorkloadFixture w;

    // seed
    for (int i = 0; i < n / 2; ++i) {
        int64_t price  = sample_price();
        int64_t volume = 1 + rng() % 100;
        w.seed_prices.push_back(price);
        w.seed_volumes.push_back(volume);
        live_orders.push_back({price, volume});
    }

    w.events.reserve(n);
    for (int i = 0; i < n; ++i) {
        int roll = rng() % 10;
        if (roll < add_pct || live_orders.empty()) {
            int64_t price  = sample_price();
            int64_t volume = 1 + rng() % 100;
            w.events.push_back({ Op::Add, price, volume, 0, 0 });
            live_orders.push_back({price, volume});
        } else if (roll < add_pct + delete_pct) {
            size_t idx = rng() % live_orders.size();
            auto [price, volume] = live_orders[idx];
            w.events.push_back({ Op::Delete, price, volume, 0, 0 });
            live_orders[idx] = live_orders.back();
            live_orders.pop_back();
        } else {
            size_t  idx        = rng() % live_orders.size();
            int64_t new_price  = sample_price();
            int64_t new_volume = 1 + rng() % 100;
            auto [old_price, old_volume] = live_orders[idx];
            w.events.push_back({ Op::Modify, new_price, new_volume, old_price, old_volume });
            live_orders[idx] = {new_price, new_volume};
        }
    }
    return w;
}