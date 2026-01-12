#pragma once
#include <char.hpp>
#include <vector>

struct ConcretePlace {
    size_t left;
    size_t right;
    std::string context;

    bool showMe = false;

    bool operator<(const ConcretePlace& rhs) const {
        return left < rhs.left;
    }
};
struct SearchResult {
    // Для корректного открытия
    std::string file_string_for_reader;
    // Для корректного отображения
    std::string file;
    std::vector<ConcretePlace> places;
    void loadContextFor(ConcretePlace& place);
};