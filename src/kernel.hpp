#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "defs.hpp"
#include "file.hpp"
#include "indexes.hpp"
#include "util.hpp"

namespace BookManager {
    using std::cin, std::cout, std::string, std::vector;

    inline void insertBook() noexcept {
        Book book;
        {
            string input;
            cout << "输入书号：";
            cin >> input;
            if (input.size() != ISBN_LENGTH - 1) {
                cout << "错误：书号长度应为 " << ISBN_LENGTH - 1 << " 位！\n";
                return;
            }
            book.setISBN(input);
            cout << "输入书名：";
            cin >> input;
            book.setTitle(input);
            cout << "输入作者名：";
            cin >> input;
            book.setAuthor(input);
        }
        {
            cout << "输入价格：";
            cin >> book.price;
        }
        {
            u64 temp;
            cout << "输入购买日期（YYYY MM DD）：";
            cin >> temp;
            book.buyDate.year = static_cast<u16>(temp);
            cin >> temp;
            book.buyDate.month = static_cast<u8>(temp);
            cin >> temp;
            book.buyDate.day = static_cast<u8>(temp);
        }
        FilePointer ptr = File::addBook(book);
        appendIndex(book, ptr);
        cout << "插入成功！\n";
    }
    
    inline void deleteBook() noexcept {
        string input;
        cout << "输入书号：";
        cin >> input;
        FilePointer ptr = getBookByISBN(input);
        if (!ptr) {
            cout << "查无此书！\n";
            return;
        }
        Book book;
        if (!File::getBook(ptr, book)) {
            cout << "读取失败！\n";
            return;
        }
        static_cast<void>(File::deleteBook(ptr));
        removeIndex(book, ptr);
        showBooks({book});
        cout << "删除成功！\n";
    }
    
    inline void updateBook() noexcept {
        string input;
        cout << "输入书号：";
        cin >> input;
        FilePointer ptr = getBookByISBN(input);
        if (!ptr) {
            cout << "查无此书！\n";
            return;
        }
        Book book, oldBook;
        if (!File::getBook(ptr, book)) {
            cout << "读取失败！\n";
            return;
        }
        oldBook = book;
        showBooks({book});
        {
            cout << "输入新书号：";
            cin >> input;
            if (input.size() != ISBN_LENGTH - 1) {
                cout << "错误：书号长度应为 " << ISBN_LENGTH - 1 << " 位！\n";
                return;
            }
            book.setISBN(input);
            cout << "输入新书名：";
            cin >> input;
            book.setTitle(input);
            cout << "输入新作者名：";
            cin >> input;
            book.setAuthor(input);
        }
        {
            cout << "输入新价格：";
            cin >> book.price;
        }
        {
            u64 temp;
            cout << "输入新购买日期（YYYY MM DD）：";
            cin >> temp;
            book.buyDate.year = static_cast<u16>(temp);
            cin >> temp;
            book.buyDate.month = static_cast<u8>(temp);
            cin >> temp;
            book.buyDate.day = static_cast<u8>(temp);
        }
        static_cast<void>(File::deleteBook(ptr));
        removeIndex(oldBook, ptr);
        FilePointer newPtr = File::addBook(book);
        appendIndex(book, newPtr);
        cout << "更新成功！\n";
    }

    inline void searchBook() noexcept {
        string input;
        while (true) {
            cout << "1.按照书名查找 2.按照作者名查找 3.退出查找\n输入选择：";
            cin >> input;
            if (input.size() != 1) {
                cout << "无效选择。\n";
                continue;
            }
            switch (input[0]) {
                case '1': {
                    cout << "输入书名：";
                    cin >> input;
                    vector<FilePointer> bookPtrs = getBooksByTitle(input);
                    vector<Book> books;
                    for (u64 i = 0; i < bookPtrs.size(); i++) {
                        Book book;
                        if (File::getBook(bookPtrs[i], book)) books.push_back(book);
                    }
                    showBooks(books);
                    break;
                }
                case '2': {
                    cout << "输入作者名：";
                    cin >> input;
                    vector<Book> books;
                    File::scanBooks([&input, &books] (FilePointer ptr, const Book& book) {
                        if (book.getAuthor() == input) books.push_back(book);
                        return true;
                    });
                    showBooks(books);
                    break;
                }
                case '3': return;
                default: cout << "无效选择。\n"; continue;
            }
        }
    }

    inline void sortByAuthor() noexcept {
        vector<Book> books;
        File::scanBooks([&books] (FilePointer, const Book& book) {
            books.push_back(book);
            return true;
        });
        sort(books.begin(), books.end(), [] (const Book& a, const Book& b) {
            return a.getAuthor() < b.getAuthor();
        });
        File::nukeBooks(books);
        buildIndices();
        showBooks(books);
    }
}