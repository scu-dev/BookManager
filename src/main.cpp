#include "utils.hpp"

int main(int argc, char** argv) {
    #if BM_WINDOWS
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    #endif
    
    return 0;
}