#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "book.hpp"
#include "file.hpp"

namespace BookManager {
    typedef int32_t i32;
    typedef uint64_t u64;
    using std::string, std::vector, std::fstream, std::cout, std::cin, std::endl, std::setw, std::left, std::ios, std::lower_bound, std::stringstream;

    inline vector<ISBNIndex> numIndexTable;
    inline vector<TitleIndex> titleIndexTable;

    inline void Display(const Book& book) noexcept {
        cout << left << setw(15) << book.getISBN() << setw(22) << book.getTitle() << setw(12) << book.getAuthor() << setw(8) << book.price << book.buyDate.year << "-" << static_cast<i32>(book.buyDate.month) << "-" << static_cast<i32>(book.buyDate.day) << endl;
    }

    inline void InsertHelp(Book& book, FilePointer offset) noexcept {
        ISBNIndex numItem;
        numItem.isbn = book.isbn;
        numItem.offset = offset;
        auto itNum = lower_bound(numIndexTable.begin(), numIndexTable.end(), numItem);
        numIndexTable.insert(itNum, numItem);
        TitleIndex titleItem;
        titleItem.title = book.title;
        titleItem.head = offset;
        auto itTitle = lower_bound(titleIndexTable.begin(), titleIndexTable.end(), titleItem);
        if (itTitle != titleIndexTable.end() && itTitle->title == book.title) itTitle->head = offset;
        else titleIndexTable.insert(itTitle, titleItem);
    }

    inline void Insert() noexcept {
        Book book;
        string input;
        cout << "输入书号：";
        getline(cin, input);
        book.setISBN(input);
        auto itCheck = lower_bound(numIndexTable.begin(), numIndexTable.end(), book.getISBN(), [](const ISBNIndex& item, string_view val) {
            return item < val;
        });
        if (itCheck != numIndexTable.end() && itCheck->getISBN() == book.getISBN()) {
            cout << "错误：书号已存在！" << endl;
            return;
        }
        cout << "输入书名：";
        getline(cin, input);
        book.setTitle(input);
        cout << "输入作者名：";
        getline(cin, input);
        book.setAuthor(input);
        cout << "输入价格：";
        getline(cin, input);
        try { book.price = stof(input); }
        catch(...) { book.price = 0; }
        cout << "输入购买日期(YYYY MM DD)：";
        getline(cin, input);
        stringstream ss(input);
        i32 y, m, d;
        ss >> y >> m >> d;
        book.buyDate.year = static_cast<u16>(y);
        book.buyDate.month = static_cast<u8>(m);
        book.buyDate.day = static_cast<u8>(d);
        auto itTitle = lower_bound(titleIndexTable.begin(), titleIndexTable.end(), book.getTitle(), [](const TitleIndex& item, string_view val) {
            return item < val;
        });
        if (itTitle != titleIndexTable.end() && itTitle->title == book.title) book.titleNext = itTitle->head;
        else book.titleNext = FilePointer(-1);
        FilePointer offset = allocBook();
        static_cast<void>(writeBook(offset, book));
        InsertHelp(book, offset);
        cout << "插入成功！" << endl;
    }

    inline void UnlinkTitle(const Book& book, FilePointer offset) noexcept {
        auto itTitle = lower_bound(titleIndexTable.begin(), titleIndexTable.end(), book.getTitle(), [](const TitleIndex& item, string_view val) {
            return item < val;
        });
        if (itTitle != titleIndexTable.end() && itTitle->title == book.title) {
            if (itTitle->head == offset) {
                if (book.titleNext) {
                    itTitle->head = book.titleNext;
                } else {
                    titleIndexTable.erase(itTitle);
                }
            } else {
                FilePointer curr = itTitle->head;
                Book b;
                while (curr) {
                    if (readBook(curr, b)) {
                        if (b.titleNext == offset) {
                            b.titleNext = book.titleNext;
                            static_cast<void>(writeBook(curr, b));
                            break;
                        }
                        curr = b.titleNext;
                    } else break;
                }
            }
        }
    }

    inline void Delete() noexcept {
        cout << "输入书号：";
        string num;
        getline(cin, num);
        auto it = lower_bound(numIndexTable.begin(), numIndexTable.end(), num, [](const ISBNIndex& item, string_view val) {
            return item < val;
        });
        if (it == numIndexTable.end() || it->getISBN() != num) {
            cout << "未找到该书号！" << endl;
            return;
        }
        FilePointer offset = it->offset;
        Book book;
        if (!readBook(offset, book)) {
            cout << "读取失败！" << endl;
            return;
        }
        UnlinkTitle(book, offset);
        numIndexTable.erase(it);
        static_cast<void>(deleteBook(offset));
        cout << "删除成功！" << endl;
    }

    inline void Update() noexcept {
        cout << "输入书号：";
        string num;
        getline(cin, num);
        auto it = lower_bound(numIndexTable.begin(), numIndexTable.end(), num, [](const ISBNIndex& item, string_view val) {
            return item < val;
        });
        if (it == numIndexTable.end() || it->getISBN() != num) {
            cout << "未找到该书号！" << endl;
            return;
        }
        FilePointer offset = it->offset;
        Book oldBook;
        if (!readBook(offset, oldBook)) return;
        UnlinkTitle(oldBook, offset);
        numIndexTable.erase(it);
        static_cast<void>(deleteBook(offset));
        Book newBook = oldBook;
        newBook.isDeleted = false;
        string input;
        cout << "输入新书名 (" << oldBook.getTitle() << ")："; 
        getline(cin, input);
        if(!input.empty()) newBook.setTitle(input);
        cout << "输入新作者 (" << oldBook.getAuthor() << ")：";
        getline(cin, input);
        if(!input.empty()) newBook.setAuthor(input);
        cout << "输入新价格 (" << oldBook.price << ")：";
        getline(cin, input);
        if(!input.empty()) newBook.price = stof(input);
        auto itTitle = lower_bound(titleIndexTable.begin(), titleIndexTable.end(), newBook.getTitle(), [](const TitleIndex& item, string_view val) {
            return item < val;
        });
        if (itTitle != titleIndexTable.end() && itTitle->title == newBook.title) newBook.titleNext = itTitle->head;
        else newBook.titleNext = FilePointer{};
        FilePointer newOffset = allocBook();
        static_cast<void>(writeBook(newOffset, newBook));
        InsertHelp(newBook, newOffset);
        cout << "更新成功！" << endl;
    }

    inline void SearchByTile() noexcept {
        cout << "输入书名：";
        string title;
        getline(cin, title);
        auto it = lower_bound(titleIndexTable.begin(), titleIndexTable.end(), title, [](const TitleIndex& item, string_view val) {
            return item < val;
        });
        if (it == titleIndexTable.end() || it->getTitle() != title) {
            cout << "查无此书！" << endl;
            return;
        }
        FilePointer current = it->head;
        while (current) {
            Book book;
            if (readBook(current, book)) {
                if (!book.isDeleted) Display(book);
                current = book.titleNext;
            }
            else break;
        }
    }

    inline void SearchByAuthor() noexcept {
        cout << "输入作者名：";
        string author;
        getline(cin, author);
        bool found = false;
        scanBooks([&](const Book& book, FilePointer) {
            if (!book.isDeleted && book.getAuthor() == author) {
                Display(book);
                found = true;
            }
        });
        if (!found) cout << "查无此人！" << endl;
    }

    inline void Search() noexcept {
        cout << "1.按照书名查找 2.按照作者名查找\n输入选择：";
        string choice;
        getline(cin, choice);
        if (choice == "1") SearchByTile();
        else if (choice == "2") SearchByAuthor();
    }

    inline void SortByAuthor() noexcept {
        vector<Book> books;
        scanBooks([&](const Book& book, FilePointer) {
            if (!book.isDeleted) books.push_back(book);
        });
        sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
            return a.getAuthor() < b.getAuthor();
        });
        for (u64 i = 0; i < books.size(); i++) Display(books[i]);
    }

    inline void rebuildIndices() noexcept {
        numIndexTable.clear();
        titleIndexTable.clear();
        scanBooks([](const Book& book, FilePointer offset) {
            if (!book.isDeleted) {
                ISBNIndex numItem;
                numItem.isbn = book.isbn;
                numItem.offset = offset;
                numIndexTable.push_back(numItem);
                bool found = false;
                for (u64 i = 0; i < titleIndexTable.size(); i++) if (titleIndexTable[i].title == book.title) {
                    titleIndexTable[i].head = offset; 
                    found = true;
                    break;
                }
                if (!found) {
                    TitleIndex titleItem;
                    titleItem.title = book.title;
                    titleItem.head = offset;
                    titleIndexTable.push_back(titleItem);
                }
            }
        });
        sort(numIndexTable.begin(), numIndexTable.end());
        sort(titleIndexTable.begin(), titleIndexTable.end());
    }

    [[nodiscard]] inline bool init() noexcept {
        if (!openFile()) return false;
        rebuildIndices();
        return true;
    }

    inline void shutdown() noexcept { closeFile(); }

    inline void processInput(const string& input) noexcept {
        if (input.size() != 1) {
            cout << "无效选择" << endl;
            return;
        }
        switch (input[0]) {
            case '1':
                Insert();
                break;
            case '2':
                Delete();
                break;
            case '3':
                Update();
                break;
            case '4':
                Search();
                break;
            case '5':
                SortByAuthor();
                break;
            default:
                cout << "无效选择" << endl;
                break;
        }
    }
}