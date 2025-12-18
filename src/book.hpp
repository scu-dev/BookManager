#pragma once
#include <algorithm>
#include <array>
#include <string>
#include <string_view>

namespace BookManager {
    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef int64_t i64;
    using std::array, std::string, std::string_view, std::memset, std::memcpy, std::min;

    constexpr size_t ISBN_LEN = 14, TITLE_LEN = 61, AUTHOR_LEN = 31;

    struct FilePointer {
        i64 value{-1};

        [[nodiscard]] FilePointer() noexcept = default;
        [[nodiscard]] explicit FilePointer(i64 value) noexcept : value(value) {}

        [[nodiscard]] bool operator==(const FilePointer& other) const noexcept { return value == other.value; }
        [[nodiscard]] bool operator!=(const FilePointer& other) const noexcept { return value != other.value; }
        [[nodiscard]] operator bool() const noexcept { return value != -1; }
    };

    struct DateType {
        u16 year{0};
        u8 month{0}, day{0};
    };

    struct Book {
        array<char, ISBN_LEN> isbn{};
        array<char, TITLE_LEN> title{};
        FilePointer titleNext{-1}; // Pointer to next book with same title
        array<char, AUTHOR_LEN> author{};
        float price{0.0f};
        DateType buyDate{};
        bool isDeleted{false};

        void setISBN(string_view s) noexcept {
            memset(isbn.data(), 0, ISBN_LEN);
            memcpy(isbn.data(), s.data(), min(s.size(), ISBN_LEN - 1));
        }
        void setTitle(string_view s) noexcept {
            memset(title.data(), 0, TITLE_LEN);
            memcpy(title.data(), s.data(), min(s.size(), TITLE_LEN - 1));
        }
        void setAuthor(string_view s) noexcept {
            memset(author.data(), 0, AUTHOR_LEN);
            memcpy(author.data(), s.data(), min(s.size(), AUTHOR_LEN - 1));
        }

        [[nodiscard]] string getISBN() const noexcept { return string(isbn.data()); }
        [[nodiscard]] string getTitle() const noexcept { return string(title.data()); }
        [[nodiscard]] string getAuthor() const noexcept { return string(author.data()); }
    };

    struct ISBNIndex {
        array<char, ISBN_LEN> isbn{};
        FilePointer offset{-1};

        [[nodiscard]] string getISBN() const noexcept { return string(isbn.data()); }

        auto operator<=>(const ISBNIndex& other) const noexcept { return string_view(isbn.data()) <=> string_view(other.isbn.data()); }
        bool operator<(string_view otherISBN) const noexcept { return string_view(isbn.data()) < otherISBN; }
        bool operator>(string_view otherISBN) const noexcept { return string_view(isbn.data()) > otherISBN; }
        bool operator==(string_view otherISBN) const noexcept { return string_view(isbn.data()) == otherISBN; }
    };

    struct TitleIndex {
        array<char, TITLE_LEN> title{};
        FilePointer head{-1}; 

        [[nodiscard]] string getTitle() const noexcept { return string(title.data()); }

        auto operator<=>(const TitleIndex& other) const noexcept { return string_view(title.data()) <=> string_view(other.title.data()); }
        bool operator<(string_view otherTitle) const noexcept { return string_view(title.data()) < otherTitle; }
        bool operator>(string_view otherTitle) const noexcept { return string_view(title.data()) > otherTitle; }
        bool operator==(string_view otherTitle) const noexcept { return string_view(title.data()) == otherTitle; }
    };
}