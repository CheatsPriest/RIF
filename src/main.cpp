#include <iostream>
#include "configs/SearchConfig.hpp"
#include "folders_inspector/FoldersInspector.hpp"

#include <core/Core.hpp>

#include <fstream>
#include <libstemmer.h>

std::string cp1251_to_utf8(const std::string& cp1251_str) {
    if (cp1251_str.empty()) return std::string();

    // 1. CP1251 → UTF-16
    int wchars_count = MultiByteToWideChar(1251, 0, cp1251_str.c_str(), -1, nullptr, 0);
    if (wchars_count == 0) return std::string();

    std::vector<wchar_t> utf16_str(wchars_count);
    MultiByteToWideChar(1251, 0, cp1251_str.c_str(), -1, utf16_str.data(), wchars_count);

    // 2. UTF-16 → UTF-8
    int utf8_count = WideCharToMultiByte(CP_UTF8, 0, utf16_str.data(), -1,
        nullptr, 0, nullptr, nullptr);
    if (utf8_count == 0) return std::string();

    std::vector<char> utf8_str(utf8_count);
    WideCharToMultiByte(CP_UTF8, 0, utf16_str.data(), -1,
        utf8_str.data(), utf8_count, nullptr, nullptr);

    return std::string(utf8_str.data());
}

// Конвертация UTF-8 -> CP1251 (если нужно)
std::string utf8_to_cp1251(const std::string& utf8_str) {
    if (utf8_str.empty()) return std::string();

    // 1. UTF-8 → UTF-16
    int wchars_count = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, nullptr, 0);
    if (wchars_count == 0) return std::string();

    std::vector<wchar_t> utf16_str(wchars_count);
    MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, utf16_str.data(), wchars_count);

    // 2. UTF-16 → CP1251
    int cp1251_count = WideCharToMultiByte(1251, 0, utf16_str.data(), -1,
        nullptr, 0, nullptr, nullptr);
    if (cp1251_count == 0) return std::string();

    std::vector<char> cp1251_str(cp1251_count);
    WideCharToMultiByte(1251, 0, utf16_str.data(), -1,
        cp1251_str.data(), cp1251_count, nullptr, nullptr);

    return std::string(cp1251_str.data());
}

// Обертка для стеммера с автоматической конвертацией
std::string stem_russian_word_correct(const std::string& word_cp1251) {
    static sb_stemmer* stemmer = nullptr;

    if (!stemmer) {
        stemmer = sb_stemmer_new("russian", "UTF_8");
        if (!stemmer) {
            std::cout << "Не удалось создать стеммер" << std::endl;
            return word_cp1251;
        }
    }

    // Конвертируем из CP1251 в UTF-8
    std::string word_utf8 = cp1251_to_utf8(word_cp1251);

    // Стемминг
    const unsigned char* result = sb_stemmer_stem(
        stemmer,
        reinterpret_cast<const unsigned char*>(word_utf8.c_str()),
        static_cast<int>(word_utf8.size())
    );

    if (result) {
        // Результат в UTF-8, конвертируем обратно в CP1251
        std::string result_utf8(reinterpret_cast<const char*>(result));
        return utf8_to_cp1251(result_utf8);
    }

    return word_cp1251;
}

class RussianStemmer {
private:
    sb_stemmer* stemmer;

    // Конвертация CP1251 -> UTF-8
    static std::string to_utf8(const std::string& cp1251) {
        int wlen = MultiByteToWideChar(1251, 0, cp1251.c_str(), -1, nullptr, 0);
        std::wstring utf16(wlen, 0);
        MultiByteToWideChar(1251, 0, cp1251.c_str(), -1, &utf16[0], wlen);

        int utf8len = WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::string utf8(utf8len, 0);
        WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, &utf8[0], utf8len, nullptr, nullptr);

        return utf8;
    }

    // Конвертация UTF-8 -> CP1251
    static std::string from_utf8(const std::string& utf8) {
        int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
        std::wstring utf16(wlen, 0);
        MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &utf16[0], wlen);

        int cp1251len = WideCharToMultiByte(1251, 0, utf16.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::string cp1251(cp1251len, 0);
        WideCharToMultiByte(1251, 0, utf16.c_str(), -1, &cp1251[0], cp1251len, nullptr, nullptr);

        return cp1251;
    }

public:
    RussianStemmer() {
        stemmer = sb_stemmer_new("russian", "UTF_8");
    }

    ~RussianStemmer() {
        if (stemmer) sb_stemmer_delete(stemmer);
    }

    std::string stem(const std::string& word_cp1251) {
        if (!stemmer || word_cp1251.empty()) return word_cp1251;

        // Конвертируем в UTF-8
        std::string word_utf8 = to_utf8(word_cp1251);

        // Стемминг
        const unsigned char* result = sb_stemmer_stem(
            stemmer,
            reinterpret_cast<const unsigned char*>(word_utf8.c_str()),
            static_cast<int>(word_utf8.size() - 1)  // -1 чтобы убрать нулевой байт
        );

        if (result) {
            // Конвертируем результат обратно в CP1251
            std::string result_utf8(reinterpret_cast<const char*>(result));
            return from_utf8(result_utf8);
        }

        return word_cp1251;
    }
};

int main() {


    {
        RussianStemmer stemmer;

        // Теперь русские слова в CP1251 будут правильно стеммиться
        std::cout << "соединения" << " -> " << stemmer.stem("соединения") << std::endl;
        std::cout << "программирование" << " -> " << stemmer.stem("программирование") << std::endl;
        std::cout << "читающий" << " -> " << stemmer.stem("читающий") << std::endl;
    }

    /*Core core;
    int prob;

    SearchConfig::get().amount_of_search_threads = 1;
    core.resizeSearchPool();
    
    core.startSeacrhing();

    std::cin >> prob;*/


    
    
    /*duckx::Document doc("C://src/12.docx");

    doc.open();
    int par = 0;
    for (auto p : doc.paragraphs())
    {
        par++;
        for (auto r : p.runs())
        {
            std::cout << par <<" : "<<format(r.get_text()) << std::endl;
        }
            
    }
        
    std::cout << "END" << std::endl;*/

   /* try {
        UnifiedReader reader("C://src/12.docx");

        while (!reader.empty()) {
            std::cout << reader.readWord() << std::endl;
            reader.moveToNextWord();
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }*/

    /*try {
        UnifiedReader reader("C://src/1.txt");

        std::cout << "File size: " << reader.size() << " bytes\n";

        while (!reader.empty()) {
            std::cout << reader.readWord() << std::endl;
            reader.moveToNextWord();
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
	
	FoldersInspector insepctor;

	insepctor.walk();*/
    

	return 0;
}