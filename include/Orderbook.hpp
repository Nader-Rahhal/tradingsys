#pragma once
#include <concepts>
#include <cstdint>

template <typename T>
concept Orderbook = requires(T book, int64_t price, int64_t volume) {
    { book.addOrder(price, volume) }                     -> std::same_as<void>;
    { book.deleteOrder(price, volume) }                  -> std::same_as<void>;
    { book.modifyOrder(price, volume, price, volume) }   -> std::same_as<void>;
    { book.bestPrice() }                                 -> std::same_as<int64_t>;
    { book.volumeAt(price) }                             -> std::same_as<int64_t>;
};