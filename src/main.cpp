#include <iostream>
#include "MapBook.hpp"

enum class Side { Bid, Ask };

struct BidTag { using Book = MapBook<std::greater<int64_t>>; };
struct AskTag { using Book = MapBook<std::less<int64_t>>;    };

int main() {
    
    BidTag::Book bids;
    AskTag::Book asks;

    bids.addOrder(1, 100'00, 10);
    bids.addOrder(2,  99'00, 5);
    asks.addOrder(3, 101'00, 8);
    asks.addOrder(4, 102'00, 3);

    std::cout << "Best bid: " << bids.bestPrice() / 100.0 << " vol: "     << bids.volumeAt(bids.bestPrice()) << "\n";
    std::cout << "Best ask: " << asks.bestPrice() / 100.0 << " vol: "     << asks.volumeAt(asks.bestPrice()) << "\n";

    return 0;
}