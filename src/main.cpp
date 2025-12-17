#include <atomic>
#include <iostream>
#include <string>

#include "file.hpp"
#include "kernel.hpp"
#include "meta.hpp"
#include "util.hpp"

int main(int argc, char** argv) {
    using std::cin, std::cout, std::string, std::memory_order_acquire, BookManager::gotCtrlC, BookManager::BM_COPYRIGHT_NOTICE, BookManager::BM_APP_NAME, BookManager::BM_SEMATIC_VERSION, BookManager::getPrompt, BookManager::processInput, BookManager::openFile, BookManager::closeFile;

    #if BM_WINDOWS
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    #endif
    if (!openFile()) {
        cout << "无法打开数据文件！\n";
        return 1;
    }
    string input, output;
    cout << "欢迎使用简单个人图书管理系统。\n" << BM_APP_NAME << ' ' << BM_SEMATIC_VERSION << '\n' << BM_COPYRIGHT_NOTICE << '\n';
    while (true) {
        if (gotCtrlC.load(memory_order_acquire)) break;
        cout << getPrompt();
        if (!getline(cin, input)) {
            if (gotCtrlC.load(memory_order_acquire)) break;
            if (cin.eof()) break;
            cin.clear();
            continue;
        }
        if (input == "6") break;
        if (processInput(input, output)) [[likely]] {
            if (!output.empty()) cout << output << '\n';
        }
        else [[unlikely]] cout << "错误：" << output << '\n';
    }
    closeFile();
    return 0;
}