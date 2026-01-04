#pragma once
#include <algorithm>
#include <format>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "defs.hpp"

namespace BookManager {
    typedef uint64_t u64;
    using std::max, std::cout, std::format, std::setw, std::left, std::fixed, std::setprecision, std::string, std::vector, std::memory_order_release;

    inline u64 getWidth(string_view data) noexcept {
        u64 result = 0;
        for (u64 i = 0; i < data.size(); i++) {
            if (data[i] & 0x80) {
                result += 2;
                if ((data[i] & 0xE0) == 0xC0) i += 1;
                else if ((data[i] & 0xF0) == 0xE0) i += 2;
                else if ((data[i] & 0xF8) == 0xF0) i += 3;
            }
            else result++;
        }
        return result;
    }

    inline void showBooks(const vector<Book>& books) noexcept {
        u64 maxTitleLen = 0, maxAuthorLen = 0, maxPriceLen = 0;
        string tempPriceStr;
        for (u64 i = 0; i < books.size(); i++) {
            maxTitleLen = max(maxTitleLen, getWidth(books[i].getTitle()));
            maxAuthorLen = max(maxAuthorLen, getWidth(books[i].getAuthor()));
            tempPriceStr = format("{:.2f}", books[i].price);
            maxPriceLen = max(maxPriceLen, tempPriceStr.size());
        }
        cout << format("{:^{}}", "书号", 13) << "  "
             << format("{:^{}}", "书名", maxTitleLen) << "  "
             << format("{:^{}}", "作者", maxAuthorLen) << "  "
             << format("{:^{}}", "价格", maxPriceLen) << "  "
             << format("{:^{}}", "购买日期", 10)
             << "\n";
        for (u64 i = 0; i < books.size(); i++) if(!books[i].isDeleted) cout
            << format("{:^{}}", books[i].getISBN(), 13) << "  "
            << format("{:^{}}", books[i].getTitle(), maxTitleLen) << "  "
            << format("{:^{}}", books[i].getAuthor(), maxAuthorLen) << "  "
            << format("{:^{}.2f}", books[i].price, maxPriceLen) << "  "
            << format("{:04}-{:02}-{:02}", books[i].buyDate.year, static_cast<u64>(books[i].buyDate.month), static_cast<u64>(books[i].buyDate.day))
            << "\n";
    }
}