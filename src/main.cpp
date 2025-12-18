#include <iostream>
#include <string>

#include "kernel.hpp"
#include "util.hpp"

int main() {
    using std::cin, std::cout, std::string, BookManager::init, BookManager::shutdown, BookManager::processInput;

    #if BM_WINDOWS
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
    if (!init()) {
        cout << "无法打开数据文件！\n";
        return 1;
    }
    cout << "欢迎使用简单个人图书管理系统。\n";
    string input;
    while (true) {
        cout << "1.插入记录 2.删除记录 3.更新记录 4.查找记录 5.按作者名排序 6.退出\n输入选择：";
        if (!getline(cin, input)) break;
        if (input == "6") break;
        processInput(input);
    }
    shutdown();
    return 0;
}