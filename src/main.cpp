#include <iostream>
#include "MapBook.hpp"

enum class Side { Bid, Ask };

struct BidTag { using Book = MapBook<std::greater<int64_t>>; };
struct AskTag { using Book = MapBook<std::less<int64_t>>;    };

int main() {
    
    return 0;
}