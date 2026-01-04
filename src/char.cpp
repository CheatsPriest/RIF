#include <char.hpp>

#ifdef _WIN32
#include <windows.h> 
#include <vector>

std::string utf8_to_cp1251(std::string&& utf8) {
    if (utf8.empty()) return "";

    int wsize = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
    if (wsize == 0) return "";

    std::wstring utf16(wsize, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &utf16[0], wsize);

    int size = WideCharToMultiByte(1251, 0, utf16.c_str(), -1, NULL, 0, NULL, NULL);
    if (size == 0) return "";

    std::string cp1251(size, 0);
    WideCharToMultiByte(1251, 0, utf16.c_str(), -1, &cp1251[0], size, NULL, NULL);

    cp1251.pop_back();
    return cp1251;
}

std::string cp1251_to_utf8(std::string&& cp1251_str) {
    if (cp1251_str.empty()) return std::string();

    int wchars_count = MultiByteToWideChar(1251, 0, cp1251_str.c_str(), -1, nullptr, 0);
    if (wchars_count == 0) return std::string();

    std::vector<wchar_t> utf16_str(wchars_count);
    MultiByteToWideChar(1251, 0, cp1251_str.c_str(), -1, utf16_str.data(), wchars_count);

    int utf8_count = WideCharToMultiByte(CP_UTF8, 0, utf16_str.data(), -1,
        nullptr, 0, nullptr, nullptr);
    if (utf8_count == 0) return std::string();

    std::vector<char> utf8_str(utf8_count);
    WideCharToMultiByte(CP_UTF8, 0, utf16_str.data(), -1,
        utf8_str.data(), utf8_count, nullptr, nullptr);

    return std::string(utf8_str.data());
}



#endif

string formatToLocal(string&& in) {
#ifdef _WIN32
    std::string res = utf8_to_cp1251(std::move(in));
    return res;
#else
    return in;
#endif
}

string formatFromLocal(string&& in) {
#ifdef _WIN32
    std::string res = cp1251_to_utf8(std::move(in));
    return res;
#else
    return in;
#endif
}