#include <iostream>
#include "configs/SearchConfig.hpp"
#include "folders_inspector/FoldersInspector.hpp"

#include <core/Core.hpp>

#include <fstream>
#include <search/synonymous/StemmerPipeline.hpp>

#include <preproc/PreSynonyms.hpp>

#include <unicode/ucsdet.h>
#include <unicode/ucnv.h>
#include <unicode/ustring.h>
#include <unicode/unistr.h>




std::u16string readAnyFileToUTF16(const std::string& filePath) {
    // 1. Читаем файл в бинарный буфер через STL
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) return u"";

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) return u"";

    // 2. Детектируем кодировку с помощью ICU
    UErrorCode status = U_ZERO_ERROR;
    UCharsetDetector* csd = ucsdet_open(&status);

    // Даем детектору буфер (лучше ограничить первыми 16КБ для скорости)
    ucsdet_setText(csd, buffer.data(), static_cast<int32_t>(buffer.size()), &status);

    const UCharsetMatch* match = ucsdet_detect(csd, &status);
    if (U_FAILURE(status) || !match) {
        ucsdet_close(csd);
        return u"";
    }

    const char* encodingName = ucsdet_getName(match, &status);
    std::cout << "[Log] Detected encoding: " << encodingName << std::endl;

    // 3. Конвертируем из обнаруженной кодировки в UTF-16
    UConverter* conv = ucnv_open(encodingName, &status);

    // В UTF-16 символов не может быть больше, чем байт в исходном файле
    std::u16string utf16Result;
    utf16Result.resize(buffer.size());

    // Прямая конвертация в буфер строки
    int32_t targetLen = ucnv_toUChars(conv,
        reinterpret_cast<UChar*>(&utf16Result[0]), static_cast<int32_t>(utf16Result.size()),
        buffer.data(), static_cast<int32_t>(buffer.size()), &status);

    utf16Result.resize(targetLen);

    // Чистим ресурсы
    ucnv_close(conv);
    ucsdet_close(csd);

    return utf16Result;
}

int main() {

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // Требует #include <windows.h>
    std::setvbuf(stdout, nullptr, _IOFBF, 1000); // Чтобы вывод не тормозил
#endif

    std::string path = "C:\\src\\ANSI_2051.txt"; // Укажите свой путь
    std::u16string content = readAnyFileToUTF16(path);

    if (!content.empty()) {
        std::cout << "Successfully read " << content.size() << " Unicode symbols." << std::endl;
        // Для вывода в консоль (которая обычно UTF-8) можно сконвертировать обратно
    }
    if (!content.empty()) {
        // Создаем объект UnicodeString из вашего вектора/строки
        icu::UnicodeString uStr(reinterpret_cast<const UChar*>(content.data()),
            static_cast<int32_t>(content.size()));

        std::string out;
        uStr.toUTF8String(out); // Конвертация в обычный std::string (UTF-8)

        std::cout << out << std::endl;
    }
    /*PreSynonyms pr;
    pr.run();
    for (auto& el : SynonymsSettings::get().groupId_count_read_only) {
        std::cout << el << std::endl;
    }
    for (auto& el : SynonymsSettings::get().synonyms_per_group) {
        std::cout << el.first << " " << el.second << std::endl;
    }
    auto& cfg = SynonymsSettings::get();
    cfg;*/
    //{
    //    StemmerPipeline stemmer("russian");

    //    // Теперь русские слова в CP1251 будут правильно стеммиться
    //    std::cout << "принят" << " -> " << stemmer.stem("Принят") << std::endl;
    //    std::cout << "прием" << " -> " << stemmer.stem("пРием") << std::endl;
    //    std::cout << "приемAs" << " -> " << stemmer.stem("приемAs") << std::endl;
    //}
    //auto& cfg = SearchConfig::get();
    //
    ////cfg.raw_templ = "большие деньги";
    //Core core;
    //int prob;

    //SearchConfig::get().amount_of_search_threads = 1;
    //core.resizeSearchPool();
    //
    //
    //core.startSeacrhing();


    //cfg.raw_templ = "деньги";
    ////core.startSeacrhing();

    //std::cin >> prob;


    
    
    /*duckx::Document doc("C://src/12.docx");

    doc.open();
    int par = 0;
    for (auto p : doc.paragraphs())
    {
        par++;
        for (auto r : p.runs())
        {
            std::cout << par <<" : "<<formatToLocal(r.get_text()) << std::endl;
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