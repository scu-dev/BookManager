#include <atomic>
#include <iostream>
#include <string>

#include "file.hpp"
#include "indexes.hpp"
#include "kernel.hpp"

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__NT__)
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #define BM_WINDOWS 1
    #include <windows.h>
#endif

int main() {
    using std::memory_order_acquire, std::cin, std::cout, std::string, BookManager::insertBookState, BookManager::deleteBookState, BookManager::updateBookState, BookManager::searchBookState, BookManager::sortByAuthor, BookManager::buildIndices;

    #if BM_WINDOWS
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
    cout << "欢迎使用简单个人图书管理系统。\n"
            "25 秋 数据结构课程设计 第五次实验\n"
            "LJM12914\n"
            "https://github.com/ljm12914\n";
    BookManager::File::init();
    buildIndices();
    string input;
    while (true) {
        cout << "1.插入记录 2.删除记录 3.更新记录 4.查找记录 5.按作者名排序 6.退出\n输入选择：";
        cin >> input;
        if (input == "6") break;
        if (input.size() != 1) {
            cout << "无效选择。\n";
            continue;
        }
        switch (input[0]) {
            case '1': insertBookState();     break;
            case '2': deleteBookState();     break;
            case '3': updateBookState();     break;
            case '4': searchBookState();     break;
            case '5': sortByAuthor();        break;
            default:  cout << "无效选择。\n"; break;
        }
    }
    BookManager::File::shutdown();
    return 0;
}