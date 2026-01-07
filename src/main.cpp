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
#include <windows.h>



int main() {

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // Требует #include <windows.h>
    std::setvbuf(stdout, nullptr, _IOFBF, 1000); // Чтобы вывод не тормозил
#endif

    //std::string path = "C:\\src\\ANSI_2051.txt"; // Укажите свой путь
    //std::u16string content = readAnyFileToUTF16(path);

    //if (!content.empty()) {
    //    std::cout << "Successfully read " << content.size() << " Unicode symbols." << std::endl;
    //    // Для вывода в консоль (которая обычно UTF-8) можно сконвертировать обратно
    //}
    //if (!content.empty()) {
    //    // Создаем объект UnicodeString из вашего вектора/строки
    //    icu::UnicodeString uStr(reinterpret_cast<const UChar*>(content.data()),
    //        static_cast<int32_t>(content.size()));

    //    std::string out;
    //    uStr.toUTF8String(out); // Конвертация в обычный std::string (UTF-8)

    //    std::cout << out << std::endl;
    //}
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
    auto& cfg = SearchConfig::get();
    auto& stats = SearchStats::get();
    cfg.raw_templ = u"большие деньги";
    Core core;
    int prob;

    SearchConfig::get().amount_of_search_threads = 1;
    core.resizeSearchPool();
    
    
    core.startSeacrhing();

    
    

    while (stats.process_search);

    std::cout << "Completed Search" << std::endl;
    std::cin >> prob;

    cfg.raw_templ = u"деньги";
    core.startSeacrhing();
    
    std::cin >> prob;
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

    //try {
    //    //UnifiedReader reader("C://src/12.docx");
    //    UnifiedReader reader("C://src/syn.txt");
    //    //UnifiedReader reader("C://src/UTF8.txt");
    //    /*while (!reader.empty()) {
    //        std::cout << reader.readWord() << std::endl;
    //        reader.moveToNextWord();
    //    }*/
    //    while (!reader.empty()) {
    //        std::cout << reader.readSymbol();
    //        reader.moveToSymbol(1);
    //    }

    //}
    //catch (const std::exception& e) {
    //    std::cerr << "Error: " << e.what() << std::endl;
    //}

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