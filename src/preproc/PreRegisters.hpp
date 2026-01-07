#pragma once

#include <configs/SearchConfig.hpp>
#include <format>

 

class PreRegisters {
private:
    SearchConfig& config;
    Lowercaser to_lowercase;

public:
    PreRegisters()
        : config(SearchConfig::get())
    {
    }

    void preprocess() {
        config.exact_templ = config.raw_templ;
        if (config.respect_registers) {
            return;
        }

        to_lowercase(config.exact_templ);
    }

private:

    /*std::string to_lowercase_ascii(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    */
};