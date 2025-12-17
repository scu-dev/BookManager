#pragma once
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "book.hpp"
#include "file.hpp"

namespace BookManager {
    typedef uint8_t u8;
    using std::string, std::stringstream, std::vector, std::sort, std::setw, std::fixed, std::setprecision, std::stod, std::min, std::left, std::defaultfloat;

    enum struct State : u8 {
        MAIN_MENU,
        INSERT_ISBN, INSERT_TITLE, INSERT_AUTHOR, INSERT_PRICE, INSERT_BUY_DATE,
        DELETE,
        UPDATE_SELECT, UPDATE_ISBN, UPDATE_TITLE, UPDATE_AUTHOR, UPDATE_PRICE, UPDATE_BUY_DATE,
        SEARCH_SELECT, SEARCH_TITLE, SEARCH_AUTHOR,
    };

    namespace {
        inline State s{State::MAIN_MENU};

        template<size_t N>
        inline string toString(const array<u8, N>& arr) noexcept {
            const char* ptr = reinterpret_cast<const char*>(arr.data());
            size_t len = 0;
            while (len < N && ptr[len] != '\0') len++;
            return string(ptr, len);
        }

        template<size_t N>
        inline void fromString(array<u8, N>& arr, const string& str) noexcept {
            const size_t len = min(str.length(), N);
            memcpy(arr.data(), str.data(), len);
            if (len < N) memset(arr.data() + len, 0, N - len);
        }

        [[nodiscard]] inline size_t getDisplayLength(const string& str) noexcept {
            size_t len = 0;
            for (size_t i = 0; i < str.length(); i++) {
                const u8 c = str[i];
                if (c < 0x80) len++;
                else if (c >= 0xC0) len += 2;
            }
            return len;
        }

        [[nodiscard]] inline size_t getPadWidth(const string& str, size_t width) noexcept {
            const size_t visual = getDisplayLength(str);
            return str.length() + (width > visual ? width - visual : 0);
        }
    }

    [[nodiscard]] inline State getState() noexcept { return s; }
    inline void setState(State newState) noexcept { s = newState; }

    [[nodiscard]] inline const char* getPrompt() noexcept {
        switch (s) {
            case State::MAIN_MENU:
                return "1.插入记录 2.删除记录 3.更新记录 4.查找记录 5.按作者名排序 6.退出\n输入选择：";
            case State::SEARCH_SELECT:
                return "1.按照书名查找 2.按照作者名查找 3.退出查找\n输入选择：";
            case State::INSERT_ISBN:
            case State::DELETE:
            case State::UPDATE_SELECT:
            case State::UPDATE_ISBN:
                return "输入书号：";
            case State::INSERT_TITLE:
            case State::UPDATE_TITLE:
            case State::SEARCH_TITLE:
                return "输入书名：";
            case State::INSERT_AUTHOR:
            case State::UPDATE_AUTHOR:
            case State::SEARCH_AUTHOR:
                return "输入作者名：";
            case State::INSERT_PRICE:
            case State::UPDATE_PRICE:
                return "输入价格：";
            case State::INSERT_BUY_DATE:
            case State::UPDATE_BUY_DATE:
                return "输入购买日期(YYYY MM DD)：";
            default:
                return "错误：";
        }
    }

    inline void insertBookToLists(FilePointer newPtr, Book& newBook) noexcept {
        FilePointer prev = FilePointer(-1), curr = header.firstByISBN;
        Book book;
        while (curr) {
            if (!readBook(curr, book)) break;
            if (toString(newBook.isbn) < toString(book.isbn)) break;
            prev = curr;
            curr = book.nextISBN;
        }
        newBook.nextISBN = curr;
        if (!prev) header.firstByISBN = newPtr;
        else if (readBook(prev, book)) {
            book.nextISBN = newPtr;
            static_cast<void>(writeBook(prev, book));
        }
        prev = FilePointer(-1);
        curr = header.firstByTitle;
        while (curr) {
            if (!readBook(curr, book)) break;
            if (toString(newBook.title) < toString(book.title)) break;
            prev = curr;
            curr = book.nextTitle;
        }
        newBook.nextTitle = curr;
        if (!prev) header.firstByTitle = newPtr;
        else if (readBook(prev, book)) {
            book.nextTitle = newPtr;
            static_cast<void>(writeBook(prev, book));
        }
        static_cast<void>(writeBook(newPtr, newBook));
        writeHeader();
    }

    inline void removeBookFromLists(FilePointer ptr, const Book& bookToRemove) noexcept {
        FilePointer prev = FilePointer(-1), curr = header.firstByISBN;
        Book book;
        while (curr) {
            if (curr == ptr) break;
            prev = curr;
            if (!readBook(curr, book)) break;
            curr = book.nextISBN;
        }
        if (curr) {
            if (!prev) header.firstByISBN = bookToRemove.nextISBN;
            else if (readBook(prev, book)) {
                book.nextISBN = bookToRemove.nextISBN;
                static_cast<void>(writeBook(prev, book));
            }
        }

        // Remove from Title list
        prev = FilePointer(-1);
        curr = header.firstByTitle;
        while (curr) {
            if (curr == ptr) break;
            prev = curr;
            if (!readBook(curr, book)) break;
            curr = book.nextTitle;
        }
        if (curr) {
            if (!prev) header.firstByTitle = bookToRemove.nextTitle;
            else if (readBook(prev, book)) {
                book.nextTitle = bookToRemove.nextTitle;
                static_cast<void>(writeBook(prev, book));
            }
        }
        writeHeader();
    }

    [[nodiscard]] inline FilePointer findBookByISBN(const string& isbn) noexcept {
        FilePointer curr = header.firstByISBN;
        Book book;
        while (curr) {
            if (!readBook(curr, book)) break;
            if (toString(book.isbn) == isbn) return curr;
            curr = book.nextISBN;
        }
        return FilePointer(-1);
    }

    [[nodiscard]] inline string printHeader() noexcept {
        stringstream ss;
        ss << left << setw(20) << "书号" << setw(32) << "书名" << setw(23) << "作者名" << setw(12) << "价格" << "购买日期";
        return ss.str();
    }

    [[nodiscard]] inline string printBook(const Book& b) noexcept {
        stringstream ss;
        const string isbn = toString(b.isbn), title = toString(b.title), author = toString(b.author);
        ss << left << setw(getPadWidth(isbn, 18)) << isbn << setw(getPadWidth(title, 30)) << title << setw(getPadWidth(author, 20)) << author << setw(10) << fixed << setprecision(2) << b.price << b.buyDate.year << "-" << static_cast<int>(b.buyDate.month) << "-" << static_cast<int>(b.buyDate.day);
        return ss.str();
    }

    [[nodiscard]] inline bool processInput(const string& input, string& output) noexcept {
        static Book tempBook;
        static FilePointer targetPtr;
        static Book targetBook;
        output = "";
        switch (s) {
            case State::MAIN_MENU:
                if (input.size() != 1) {
                    output = "无效输入";
                    return false;
                }
                switch (input[0]) {
                    case '1':
                        setState(State::INSERT_ISBN);
                        break;
                    case '2':
                        setState(State::DELETE);
                        break;
                    case '3':
                        setState(State::UPDATE_SELECT);
                        break;
                    case '4':
                        setState(State::SEARCH_SELECT);
                        break;
                    case '5': {
                        vector<Book> books;
                        FilePointer curr = header.firstByTitle;
                        Book book;
                        while (curr) {
                            if (!readBook(curr, book)) break;
                            books.push_back(book);
                            curr = book.nextTitle;
                        }
                        sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
                            return toString(a.author) < toString(b.author);
                        });
                        stringstream ss;
                        if (!books.empty()) ss << printHeader() << '\n';
                        for (u64 i = 0; i < books.size(); i++) ss << printBook(books[i]) << '\n';
                        output = ss.str();
                        if (output.empty()) output = "无记录";
                        break;
                    }
                    default:
                        output = "无效输入";
                        return false;
                }
                break;
            case State::INSERT_ISBN:
                if (input.length() > 13) {
                    output = "ISBN过长";
                    return false;
                }
                if (findBookByISBN(input)) {
                    output = "ISBN已存在";
                    setState(State::MAIN_MENU);
                    return false;
                }
                tempBook = Book{};
                fromString(tempBook.isbn, input);
                setState(State::INSERT_TITLE);
                break;
            case State::INSERT_TITLE:
                if (input.length() > 90) {
                    output = "书名过长";
                    return false;
                }
                fromString(tempBook.title, input);
                setState(State::INSERT_AUTHOR);
                break;
            case State::INSERT_AUTHOR:
                if (input.length() > 60) {
                    output = "作者名过长";
                    return false;
                }
                fromString(tempBook.author, input);
                setState(State::INSERT_PRICE);
                break;
            case State::INSERT_PRICE:
                try { tempBook.price = stod(input); }
                catch(...) {
                    output = "价格无效";
                    return false;
                }
                setState(State::INSERT_BUY_DATE);
                break;
            case State::INSERT_BUY_DATE: {
                stringstream ss(input);
                u16 y, m, d;
                if (!(ss >> y >> m >> d)) {
                    output = "日期格式错误";
                    return false;
                }
                tempBook.buyDate.year = y;
                tempBook.buyDate.month = static_cast<u8>(m);
                tempBook.buyDate.day = static_cast<u8>(d);
                insertBookToLists(allocBook(), tempBook);
                output = "插入成功";
                setState(State::MAIN_MENU);
                break;
            }
            case State::DELETE:
                targetPtr = findBookByISBN(input);
                if (!targetPtr) {
                    output = "未找到记录";
                    setState(State::MAIN_MENU);
                    return false;
                }
                if (!readBook(targetPtr, targetBook)) {
                    output = "读取记录失败";
                    return false;
                }
                removeBookFromLists(targetPtr, targetBook);
                static_cast<void>(deleteBook(targetPtr));
                output = "删除成功";
                setState(State::MAIN_MENU);
                break;
            case State::UPDATE_SELECT: {
                targetPtr = findBookByISBN(input);
                if (!targetPtr) {
                    output = "未找到记录"; setState(State::MAIN_MENU);
                    return false;
                }
                if (!readBook(targetPtr, targetBook)) {
                    output = "读取记录失败";
                    return false;
                }
                stringstream ss;
                ss << printHeader() << "\n" << printBook(targetBook);
                output = ss.str();
                setState(State::UPDATE_ISBN);
                break;
            }
            case State::UPDATE_ISBN:
                if (input != toString(targetBook.isbn) && findBookByISBN(input)) {
                    output = "ISBN已存在";
                    return false;
                }
                removeBookFromLists(targetPtr, targetBook);
                fromString(targetBook.isbn, input);
                insertBookToLists(targetPtr, targetBook);
                output = "更新成功";
                setState(State::UPDATE_TITLE);
                break;
            case State::UPDATE_TITLE:
                removeBookFromLists(targetPtr, targetBook);
                fromString(targetBook.title, input);
                insertBookToLists(targetPtr, targetBook);
                output = "更新成功";
                setState(State::UPDATE_AUTHOR);
                break;
            case State::UPDATE_AUTHOR:
                fromString(targetBook.author, input);
                static_cast<void>(writeBook(targetPtr, targetBook));
                output = "更新成功";
                setState(State::UPDATE_PRICE);
                break;
            case State::UPDATE_PRICE:
                try { targetBook.price = stod(input); }
                catch(...) {
                    output = "价格无效";
                    return false;
                }
                static_cast<void>(writeBook(targetPtr, targetBook));
                output = "更新成功";
                setState(State::UPDATE_BUY_DATE);
                break;
            case State::UPDATE_BUY_DATE: {
                stringstream ss(input);
                u16 y, m, d;
                if (!(ss >> y >> m >> d)) {
                    output = "日期格式错误";
                    return false;
                }
                targetBook.buyDate.year = y;
                targetBook.buyDate.month = static_cast<u8>(m);
                targetBook.buyDate.day = static_cast<u8>(d);
                static_cast<void>(writeBook(targetPtr, targetBook));
                output = "更新成功";
                setState(State::MAIN_MENU);
                break;
            }
            case State::SEARCH_SELECT:
                if (input == "1") setState(State::SEARCH_TITLE);
                else if (input == "2") setState(State::SEARCH_AUTHOR);
                else if (input == "3") setState(State::MAIN_MENU);
                else {
                    output = "无效选择";
                    return false;
                }
                break;
            case State::SEARCH_TITLE: {
                stringstream ss;
                FilePointer curr = header.firstByTitle;
                Book b;
                bool found = false;
                while (curr) {
                    if (!readBook(curr, b)) break;
                    if (toString(b.title).find(input) != string::npos) {
                        if (!found) {
                            ss << printHeader() << '\n';
                            found = true;
                        }
                        ss << printBook(b) << '\n';
                    }
                    curr = b.nextTitle;
                }
                output = ss.str();
                if (!found) output = "未找到";
                setState(State::SEARCH_SELECT);
                break;
            }
            case State::SEARCH_AUTHOR: {
                stringstream ss;
                FilePointer curr = header.firstByTitle;
                Book b;
                bool found = false;
                while (curr) {
                    if (!readBook(curr, b)) break;
                    if (toString(b.author).find(input) != string::npos) {
                        if (!found) {
                            ss << printHeader() << '\n';
                            found = true;
                        }
                        ss << printBook(b) << '\n';
                    }
                    curr = b.nextTitle;
                }
                output = ss.str();
                if (!found) output = "未找到";
                setState(State::SEARCH_SELECT);
                break;
            }
        }
        return true;
    }
}