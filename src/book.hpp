#pragma once
#include <array>

namespace BookManager {
    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef int64_t i64;
    using std::array;

    struct Date {
        u16 year{0};
        u8 month{0}, day{0};
    };

    struct FilePointer {
        i64 value{-1};

        [[nodiscard]] FilePointer() noexcept = default;
        [[nodiscard]] explicit FilePointer(i64 value) noexcept : value(value) {}

        [[nodiscard]] bool operator==(const FilePointer& other) const noexcept { return value == other.value; }
        [[nodiscard]] bool operator!=(const FilePointer& other) const noexcept { return value != other.value; }
        [[nodiscard]] operator bool() const noexcept { return value != -1; }
    };

    struct Book {
        //No null-terminators needed because they are fixed-length fields!
        array<u8, 13> isbn{};
        array<u8, 90> title{};
        array<u8, 60> author{};
        bool isDeleted{false};
        Date buyDate;
        double price{0.0};
        FilePointer nextTitle{-1}, nextISBN{-1};
    };
}