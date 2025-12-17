#pragma once
#include <atomic>
#include <filesystem>
#include <iostream>
#include <string>

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__NT__)
    #define BM_WINDOWS 1
    #include <windows.h> // IWYU pragma: keep
#else
    #define BM_UNIX 1
    #include <unistd.h> // IWYU pragma: keep
#endif

namespace BookManager {
    using std::string, std::filesystem::path, std::error_code, std::cout, std::endl;

    inline string errorMessage;

    [[nodiscard]] inline string getLastError() noexcept { return errorMessage; }
    inline void setError(const string& err) noexcept { errorMessage = err; }

    using std::atomic, std::memory_order_release;
    // Assuming the teacher will only use Windows :)
    #if BM_WINDOWS
        inline atomic<bool> gotCtrlC{false};

        inline BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType) {
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

    inline bool normalize(path& p) noexcept {
        error_code ec;
        p = absolute(p, ec);
        if (ec) return false;
        p = p.lexically_normal();
        p.make_preferred();
        return true;
    }
}