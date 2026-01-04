#pragma once
#include <array>
#include <algorithm>
#include <compare>
#include <fstream>
#include <ios>
#include <string_view>
#include <vector>

namespace BookManager {
    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef int64_t i64;
    typedef uint64_t u64;
    using std::array, std::fill, std::copy_n, std::min, std::fstream, std::string_view, std::vector, std::ios, std::strong_ordering;

    struct Date {
        u16 year{0};
        u8 month{0}, day{0};
    };

    inline constexpr size_t ISBN_LENGTH = 14, TITLE_LENGTH = 40, AUTHOR_LENGTH = 25;

    struct FilePointer {
        i64 value{-1};

        [[nodiscard]] FilePointer() noexcept = default;
        [[nodiscard]] explicit FilePointer(i64 value) noexcept : value(value) {}
        [[nodiscard]] FilePointer(fstream& file) noexcept : value(file.tellg()) {}

        [[nodiscard]] bool operator==(const FilePointer& other) const noexcept { return value == other.value; }
        [[nodiscard]] bool operator!=(const FilePointer& other) const noexcept { return value != other.value; }
        [[nodiscard]] explicit operator i64() const noexcept { return value; }
        [[nodiscard]] operator bool() const noexcept { return value != -1; }

        [[nodiscard]] FilePointer operator+(i64 offset) const noexcept { return FilePointer(value + offset); }
        [[nodiscard]] FilePointer operator-(i64 offset) const noexcept { return FilePointer(value - offset); }
        [[nodiscard]] FilePointer operator+(u64 offset) const noexcept { return FilePointer(value + offset); }
        [[nodiscard]] FilePointer operator-(u64 offset) const noexcept { return FilePointer(value - offset); }
    };

    struct Book {
        array<char, ISBN_LENGTH> isbn;
        array<char, TITLE_LENGTH> title;
        array<char, AUTHOR_LENGTH> author;
        bool isDeleted{false};
        Date buyDate;
        float price{0.0f};

        string_view getISBN() const noexcept { return string_view(isbn.data()); }
        void setISBN(string_view value) noexcept {
            fill(isbn.begin(), isbn.end(), '\0');
            copy_n(value.begin(), min(value.size(), ISBN_LENGTH - 1), isbn.begin());
        }

        string_view getTitle() const noexcept { return string_view(title.data()); }
        void setTitle(string_view value) noexcept {
            fill(title.begin(), title.end(), '\0');
            copy_n(value.begin(), min(value.size(), TITLE_LENGTH - 1), title.begin());
        }

        string_view getAuthor() const noexcept { return string_view(author.data()); }
        void setAuthor(string_view value) noexcept {
            fill(author.begin(), author.end(), '\0');
            copy_n(value.begin(), min(value.size(), AUTHOR_LENGTH - 1), author.begin());
        }
    };

    struct TitleIndexItem {
        array<char, TITLE_LENGTH> title;
        FilePointer offset;

        strong_ordering operator<=>(const TitleIndexItem& other) const noexcept {
            auto result = string_view(title.data()) <=> string_view(other.title.data());
            if (result != 0) return result;
            return offset.value <=> other.offset.value;
        }
    };

    struct ISBNIndexItem {
        array<char, ISBN_LENGTH> isbn;
        FilePointer offset;

        strong_ordering operator<=>(const ISBNIndexItem& other) const noexcept { return string_view(isbn.data()) <=> string_view(other.isbn.data()); }
    };
}