#pragma once
#include <functional>

#include "defs.hpp"
#include "util.hpp"
#include "file.hpp"

namespace BookManager::File {
    typedef uint64_t u64;
    using std::function, std::ios, std::streamoff;

    static fstream file;

    void init() noexcept {
        file.open("books.dat", ios::in | ios::out | ios::binary);
        if (!file.is_open()) {
            file.clear();
            file.open("books.dat", ios::out | ios::binary);
            file.close();
            file.open("books.dat", ios::in | ios::out | ios::binary);
        }
    }

    void shutdown() noexcept {
        if (file.is_open()) file.close();
    }

    [[nodiscard]] bool getBook(FilePointer ptr, Book& book) noexcept {
        file.clear();
        file.seekg(ptr.value, ios::beg);
        file.read(reinterpret_cast<char*>(&book), sizeof(Book));
        return !file.fail();
    }

    void scanBooks(function<bool(FilePointer ptr, const Book& book)> callback) noexcept {
        file.clear();
        file.seekg(0, ios::beg);
        Book book;
        while (file.read(reinterpret_cast<char*>(&book), sizeof(Book))) if (!callback(FilePointer(file) - sizeof(Book), book)) return;
    }

    [[nodiscard]] FilePointer addBook(const Book& book) noexcept {
        file.clear();
        file.seekg(0, ios::beg);
        Book temp;
        while (file.read(reinterpret_cast<char*>(&temp), sizeof(Book))) if (temp.isDeleted) {
            FilePointer ptr(file.tellg() - static_cast<streamoff>(sizeof(Book)));
            file.seekp(ptr.value, ios::beg);
            file.write(reinterpret_cast<const char*>(&book), sizeof(Book));
            file.flush();
            return ptr;
        }
        file.clear();
        file.seekp(0, ios::end);
        FilePointer ptr(file);
        file.write(reinterpret_cast<const char*>(&book), sizeof(Book));
        file.flush();
        return ptr;
    }

    [[nodiscard]] bool deleteBook(FilePointer ptr) noexcept {
        file.clear();
        file.seekp(ptr.value, ios::beg);
        Book book;
        file.read(reinterpret_cast<char*>(&book), sizeof(Book));
        if (book.isDeleted) return false;
        book.isDeleted = true;
        file.seekp(ptr.value, ios::beg);
        file.write(reinterpret_cast<const char*>(&book), sizeof(Book));
        file.flush();
        return true;
    }

    void nukeBooks(const vector<Book>& books) noexcept {
        file.clear();
        file.seekp(0, ios::beg);
        for (u64 i = 0; i < books.size(); i++) file.write(reinterpret_cast<const char*>(&books[i]), sizeof(Book));
        file.flush();
    }
}