#pragma once
#include <array>
#include <fstream>
#include <iostream>
#include <string>

#include "book.hpp"

namespace BookManager {
    typedef uint8_t u8;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::array, std::fstream, std::string, std::ios;

    namespace {
        struct Header {
            array<u8, 4> magic{'S', 'B', 'M', 0};
            u32 version{1};
            u64 bookCount{0}, activeCount{0};
            FilePointer firstByTitle, firstByISBN, firstFreeSlot;
        };

        inline constexpr const char* filename = "bookdata.bin";
        inline fstream file;
    }
    inline Header header;
    
    inline void readHeader() noexcept {
        file.seekg(0, ios::beg);
        file.read(reinterpret_cast<char*>(&header), sizeof(Header));
    }

    inline void writeHeader() noexcept {
        file.seekp(0, ios::beg);
        file.write(reinterpret_cast<const char*>(&header), sizeof(Header));
    }

    [[nodiscard]] inline bool openFile() noexcept {
        if (file.is_open()) return true;
        file.open(filename, ios::in | ios::out | ios::binary);
        if (!file.is_open()) {
            file.open(filename, ios::out | ios::binary);
            file.close();
            file.open(filename, ios::in | ios::out | ios::binary);
            if (!file.is_open()) return false;
            writeHeader();
        }
        else readHeader();
        return true;
    }

    inline void closeFile() noexcept {
        if (file.is_open()) {
            writeHeader();
            file.close();
        }
    }

    [[nodiscard]] inline bool readBook(FilePointer ptr, Book& result) noexcept {
        if (!ptr) return false;
        file.seekg(ptr.value, ios::beg);
        file.read(reinterpret_cast<char*>(&result), sizeof(Book));
        return true;
    }

    [[nodiscard]] inline bool writeBook(FilePointer ptr, const Book& book) noexcept {
        if (!ptr) return false;
        file.seekp(ptr.value, ios::beg);
        file.write(reinterpret_cast<const char*>(&book), sizeof(Book));
        return true;
    }

    [[nodiscard]] inline FilePointer allocBook() noexcept {
        FilePointer ptr;
        if (header.firstFreeSlot) {
            ptr = header.firstFreeSlot;
            Book book;
            static_cast<void>(readBook(ptr, book));
            header.firstFreeSlot = book.nextTitle;
        }
        else {
            file.seekp(0, ios::end);
            ptr = FilePointer(file.tellp());
            header.bookCount++;
        }
        header.activeCount++;
        return ptr;
    }

    [[nodiscard]] inline bool deleteBook(FilePointer ptr) noexcept {
        Book book;
        if (!readBook(ptr, book)) return false;
        book.isDeleted = true;
        book.nextTitle = header.firstFreeSlot;
        header.firstFreeSlot = ptr;
        static_cast<void>(writeBook(ptr, book));
        header.activeCount--;
        return true;
    }
}