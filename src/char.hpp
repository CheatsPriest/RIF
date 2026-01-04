#pragma once
#include <string>
#include <locale>
using char_t = char;
using string = std::string;
using string_view = std::string_view;

#ifdef _WIN32

std::string utf8_to_cp1251(std::string&& utf8);

#endif

string formatToLocal(string&& in);
string formatFromLocal(string&& in);

class Lowercaser {
private:
    std::locale sys_locale;
    void to_lowercase(std::string& str) noexcept {
        
        for (auto& c : str) {
            c = std::tolower(c, sys_locale);
        }

    }
    string to_lowercase(std::string&& str) noexcept {
        
        for (auto& c : str) {
            c = std::tolower(c, sys_locale);
        }

        return str;
    }
public:
    Lowercaser() : sys_locale(""){

    }
    void operator()(std::string& str){
        return to_lowercase(str);
    }
    string operator()(std::string&& str) {
        return to_lowercase(std::move(str));
    }
};
