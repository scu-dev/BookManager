#pragma once
#include <algorithm>
#include <string_view>
#include <vector>

#include "defs.hpp"
#include "file.hpp"

namespace BookManager {
    using std::sort, std::lower_bound, std::vector, std::string_view, File::scanBooks;

    inline vector<ISBNIndexItem> isbnIndexTable;
    inline vector<TitleIndexItem> titleIndexTable;

    inline void buildIndices() noexcept {
        isbnIndexTable.clear();
        titleIndexTable.clear();
        static_cast<void>(scanBooks([](FilePointer ptr, const Book& book) {
            if (!book.isDeleted) {
                isbnIndexTable.push_back({book.isbn, ptr});
                titleIndexTable.push_back({book.title, ptr});
            }
            return true;
        }));
        sort(isbnIndexTable.begin(), isbnIndexTable.end());
        sort(titleIndexTable.begin(), titleIndexTable.end());
    }

    inline void appendIndex(const Book& book, FilePointer ptr) noexcept {
        isbnIndexTable.push_back({book.isbn, ptr});
        titleIndexTable.push_back({book.title, ptr});
        sort(isbnIndexTable.begin(), isbnIndexTable.end());
        sort(titleIndexTable.begin(), titleIndexTable.end());
    }

    inline void removeIndex(const Book& book, FilePointer ptr) noexcept {
        auto itISBN = lower_bound(isbnIndexTable.begin(), isbnIndexTable.end(), string_view(book.isbn.data()), [](const ISBNIndexItem& item, string_view val) {
            return string_view(item.isbn.data()) < val;
        });
        if (itISBN != isbnIndexTable.end() && string_view(itISBN->isbn.data()) == string_view(book.isbn.data()) && itISBN->offset == ptr) isbnIndexTable.erase(itISBN);
        auto itTitle = lower_bound(titleIndexTable.begin(), titleIndexTable.end(), string_view(book.title.data()), [](const TitleIndexItem& item, string_view val) {
            return string_view(item.title.data()) < val;
        });
        while (itTitle != titleIndexTable.end() && string_view(itTitle->title.data()) == string_view(book.title.data())) {
            if (itTitle->offset == ptr) {
                titleIndexTable.erase(itTitle);
                break;
            }
            itTitle++;
        }
    }

    [[nodiscard]] inline FilePointer getBookByISBN(string_view isbn) noexcept {
        auto it = lower_bound(isbnIndexTable.begin(), isbnIndexTable.end(), isbn, [](const ISBNIndexItem& item, string_view val) {
            return string_view(item.isbn.data()) < val;
        });
        if (it != isbnIndexTable.end() && string_view(it->isbn.data()) == isbn) return it->offset;
        return FilePointer{};
    }

    [[nodiscard]] inline vector<FilePointer> getBooksByTitle(string_view title) noexcept {
        vector<FilePointer> results;
        auto it = lower_bound(titleIndexTable.begin(), titleIndexTable.end(), title, [](const TitleIndexItem& item, string_view val) {
            return string_view(item.title.data()) < val;
        });
        while (it != titleIndexTable.end() && string_view(it->title.data()) == title) {
            results.push_back(it->offset);
            it++;
        }
        return results;
    }
}