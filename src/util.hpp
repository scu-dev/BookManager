#pragma once
#include <atomic>

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__NT__)
    #define BM_WINDOWS 1
    #include <windows.h> // IWYU pragma: keep
#else
    #define BM_UNIX 1
    #include <unistd.h> // IWYU pragma: keep
#endif

namespace BookManager {
    using std::atomic, std::memory_order_release;

    #if BM_WINDOWS
        inline atomic<bool> gotCtrlC{false};

        inline BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType) noexcept {
            if (dwCtrlType == CTRL_C_EVENT) {
                const char crlf[] = "\r\n";
                DWORD written = 0;
                WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), crlf, static_cast<DWORD>(strlen(crlf)), &written, nullptr);
                gotCtrlC.store(true, memory_order_release);
                return TRUE;
            }
            return FALSE;
        }
    #endif
}