#include <read/docx/DocxReader.hpp>

#ifdef _WIN32
#include <windows.h> 

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

#endif

string format(string&& in) {
#ifdef _WIN32
    std::string res = utf8_to_cp1251(std::move(in));
    return res;
#endif
    return in;
}