#pragma once
#include <libstemmer.h>
#include <memory>

#include <char.hpp>



class Stemmer {
private:
    struct StemerDeleter {
        void operator()(sb_stemmer* stemmer) const {
            sb_stemmer_delete(stemmer);
        }
    };
    std::unique_ptr<sb_stemmer, StemerDeleter> stemmer_ptr;

public:
    Stemmer(std::string_view language) : stemmer_ptr(sb_stemmer_new(language.data(), "UTF_8")) {

    }

    ~Stemmer() = default;

    string stem(string&& word_cp1251);
    string_view stem(string_view word_v);

    void changeLanguage(std::string_view language);
};

