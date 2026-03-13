#pragma once

#include <concepts>

template <typename T>
concept Orderbook = requires(T book, uint64_t id, int64_t price, int64_t volume) {
    { book.addOrder(id, price, volume) } -> std::same_as<void>;
    { book.deleteOrder(id) } -> std::same_as<void>;
    { book.modifyOrder(id, price, volume) } -> std::same_as<void>;
    { book.bestPrice() } -> std::same_as<int64_t>;
    { book.volumeAt(price) } -> std::same_as<int64_t>;
};