#include <chrono>
#include <cstdio>
#include <sys/wait.h>
#include <unistd.h>
#include <csignal>

#include "Workload.hpp"
#include "MapBook.hpp"
#include "Orderbook.hpp"
#include "Vectorbook.hpp"

struct BidTag { using Book = VectorBook<std::greater<int64_t>>; };
struct AskTag { using Book = MapBook<std::less<int64_t>>;    };

auto Handler dispatch[3] = {
    [](Book& b, const Event& e) { b.addOrder(e.price, e.volume); },
    [](Book& b, const Event& e) { b.addOrder(e.price, e.volume); },
    [](Book& b, const Event& e) { b.modifyOrder(e.old_price, e.old_volume, e.price, e.volume); },
};

template <typename T>
concept OrderbookTag = Orderbook<typename T::Book>;

template <OrderbookTag Tag>
void run_perf_bench(const char* name, int n, int n_price_levels) {
    const WorkloadFixture w = make_workload(n, 42, n_price_levels, 4, 4);

    typename Tag::Book book;
    for (size_t i = 0; i < w.seed_prices.size(); ++i)
        book.addOrder(w.seed_prices[i], w.seed_volumes[i]);

    auto t0 = std::chrono::steady_clock::now();
    raise(SIGSTOP);
    for (const auto& e : w.events) {
        dispatch[static_cast<int>(e.op)](book, e);
    }

    auto t1 = std::chrono::steady_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    double ops_per_sec = n / (ms / 1000.0);
    std::printf("%-30s  n=%-10d  %.1f ms  %.2f M ops/sec\n", name, n, ms, ops_per_sec / 1e6);
}

int main() {
    run_perf_bench<BidTag>("MapBook BidTag", 10'000'000, 20);
    // run_perf_bench<AskTag>("MapBook AskTag", 10'000'000, 20);
    return 0;
}
