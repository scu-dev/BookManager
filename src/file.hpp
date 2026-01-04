#pragma once
#include <functional>

#include "defs.hpp"

namespace BookManager::File {
    using std::function;

    void init() noexcept;
    void shutdown() noexcept;

    [[nodiscard]] bool getBook(FilePointer ptr, Book& book) noexcept;
    void scanBooks(function<bool(FilePointer ptr, const Book& book)> callback) noexcept;
    [[nodiscard]] FilePointer addBook(const Book& book) noexcept;
    [[nodiscard]] bool deleteBook(FilePointer ptr) noexcept;
    void nukeBooks(const vector<Book>& books) noexcept;
}