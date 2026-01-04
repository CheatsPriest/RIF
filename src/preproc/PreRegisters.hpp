#pragma once

#include <configs/SearchConfig.hpp>
#include <locale>
#include <format>



class PreRegisters {
private:
    SearchConfig& config;
    std::locale sys_locale;

public:
    PreRegisters()
        : config(SearchConfig::get())
        , sys_locale("")  // Инициализируем один раз
    {
    }

    void preprocess() {
        if (config.respect_registers) {
            config.exact_templ = config.raw_templ;
            return;
        }

        config.exact_templ = to_lowercase(config.raw_templ);
    }

private:
    std::string to_lowercase(const std::string& str) {
        std::string result;
        result.reserve(str.size());

        for (char c : str) {
            result.push_back(std::tolower(c, sys_locale));
        }

        return result;
    }

    /*std::string to_lowercase_ascii(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    */
};