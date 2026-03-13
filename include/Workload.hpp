#pragma once
#include <random>
#include <vector>

// ── Event types ───────────────────────────────────────────────────────────────

enum class Op { Add, Delete, Modify };

struct Event {
    Op       op;
    uint64_t id;
    int64_t  price;
    int64_t  volume;
};

// ── Fixture ───────────────────────────────────────────────────────────────────

struct WorkloadFixture {
    std::vector<Event>    events;
    // orders pre-loaded into the book before the timed region
    std::vector<uint64_t> seed_ids;
    std::vector<int64_t>  seed_prices;
    std::vector<int64_t>  seed_volumes;
};

// ── Factory ───────────────────────────────────────────────────────────────────
//
// add_pct + delete_pct out of 10 — remainder is modify.
// Prices sampled with geometric decay from touch (level 0):
//   P(level k) = p * (1-p)^k,  p = 0.3
//   ~30% of activity at touch, ~70% within 3 ticks — matches liquid equity feeds.
//
// n_price_levels: number of distinct price levels above base (10000).
//   20  = liquid name, tight $0.20 spread
//   200 = wider/illiquid book
//
inline WorkloadFixture make_workload(int n, uint32_t seed, int n_price_levels,
                                     int add_pct, int delete_pct) {
    std::mt19937 rng(seed);
    std::geometric_distribution<int> level_dist(0.3);

    auto sample_price = [&]() -> int64_t {
        int level = std::min(level_dist(rng), n_price_levels - 1);
        return 10000 + level * 100;
    };

    WorkloadFixture w;
    uint64_t next_id = 1;
    std::vector<uint64_t> live_ids;

    // seed book with n/2 live orders
    for (int i = 0; i < n / 2; ++i, ++next_id) {
        int64_t price  = sample_price();
        int64_t volume = 1 + rng() % 100;
        w.seed_ids.push_back(next_id);
        w.seed_prices.push_back(price);
        w.seed_volumes.push_back(volume);
        live_ids.push_back(next_id);
    }

    w.events.reserve(n);
    for (int i = 0; i < n; ++i) {
        int roll = rng() % 10;
        if (roll < add_pct || live_ids.empty()) {
            int64_t price  = sample_price();
            int64_t volume = 1 + rng() % 100;
            w.events.push_back({ Op::Add, next_id, price, volume });
            live_ids.push_back(next_id++);
        } else if (roll < add_pct + delete_pct) {
            size_t idx = rng() % live_ids.size();
            w.events.push_back({ Op::Delete, live_ids[idx], 0, 0 });
            live_ids[idx] = live_ids.back();  // swap-and-pop O(1)
            live_ids.pop_back();
        } else {
            size_t  idx    = rng() % live_ids.size();
            int64_t price  = sample_price();
            int64_t volume = 1 + rng() % 100;
            w.events.push_back({ Op::Modify, live_ids[idx], price, volume });
        }
    }
    return w;
}