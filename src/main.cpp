#pragma execution_character_set("utf-8")

#include <iostream>
#include "configs/SearchConfig.hpp"
#include "folders_inspector/FoldersInspector.hpp"

#include <core/Core.hpp>

#include <fstream>
#include <synonymous/DeductorStemmer.hpp>

#include <preproc/PreSynonyms.hpp>

#include <unicode/ucsdet.h>
#include <unicode/ucnv.h>
#include <unicode/ustring.h>
#include <unicode/unistr.h>
#define NOMINMAX
#include <windows.h>

#include <ImGui/Window.hpp>
#include <nfd.hpp>

#include <chrono>

#include <search/caching/LRUSteamming.hpp>

#include <ImGui/ui/DesktopUi.hpp>


#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <ocr/OcrReader.hpp>

#include <ocr/OcrManager.hpp>

int main() {

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // Требует #include <windows.h>
    std::setvbuf(stdout, nullptr, _IOFBF, 1000); // Чтобы вывод не тормозил
#endif
   
    /*OcrManager& manager = OcrManager::get();
    std::string str = "C:/src/picture/picture.pdf";
    manager.push(str);
    int a;
    std::cin >> a;
    manager.restart();

    str = "C:/src/picture/1.pdf";
    manager.push(str);
    str = "C:/src/picture/2.pdf";
    manager.push(str);*/
    
    /*OcrReader reader();
    string chunk;
    while (reader.readNextChunk(chunk)) {
        std::cout << chunk << std::endl;
    }*/
    
    /*DeductorStemmer stem;
    std::cout << stem.stem_lowercased(u"helicopter") << std::endl;
    std::cout << stem.stem_lowercased(u"helicopters") << std::endl;
    std::cout << stem.stem_lowercased(u"вода") << std::endl;
    std::cout << stem.stem_lowercased(u"бойлерный") << std::endl;
    std::cout << stem.stem_lowercased(u"денег") << std::endl;
    std::cout << stem.stem_lowercased(u"деньги") << std::endl;
    std::cout << stem.stem_lowercased(u"большой") << std::endl;
    std::cout << stem.stem_lowercased(u"huge") << std::endl;
    std::cout << stem.stem_lowercased(u"огромный") << std::endl;*/

    /*SynonymsSettings::get().synonyms_per_group = { {u"машин", 0}, {u"пух", 1}};
    LRUSteamming cache(5, 100);
    for (int i = 0; i < 100; i++) {
        std::cout << cache.get(u"машина") << std::endl;
        std::cout << cache.get(u"леха") << std::endl;
        std::cout << cache.get(u"пушка") << std::endl;
        std::cout << cache.get(u"пуха") << std::endl;
        std::cout << cache.get(u"машина") << std::endl;
        std::cout << cache.get(u"душка") << std::endl;
        std::cout << cache.get(u"мушка") << std::endl;
        std::cout << cache.get(u"машина") << std::endl;
        std::cout << cache.get(u"лепеха") << std::endl;
        std::cout << cache.get(u"пух") << std::endl;
        std::cout << "====SECTION====" << std::endl;
    }*/

    DesktopUi ui;
    ui.run();

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

    //auto& cfg = SearchConfig::get();
    //auto& stats = SearchStats::get();
    //cfg.raw_templ = u"большие деньги";
    //
    //SynonymsSettings::get().use_synonyms = true;

    //Core core;
    //int prob;

    //SearchConfig::get().amount_of_search_threads = 1;
    //core.resizeSearchPool();
    //
    //for (int i = 0; i < 1; i++) {
    //    auto start = std::chrono::steady_clock::now();
    //    core.startSeacrhing();

    //    while (stats.process_search);
    //    auto end = std::chrono::steady_clock::now();


    //    std::cout << "Completed Search: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
    //}
   
    ////std::cin >> prob;

    //cfg.raw_templ = u"деньги";
    
    /*DocxReader doc("C://src/big.docx");
    string chunk;
    while (doc.readNextChunk(chunk)) {
        std::cout << chunk << std::endl;
        std::cout << chunk.size() << std::endl;
    }*/

    /*FileStreamReader doc("C://src/syn.txt");
    string chunk;
    while (doc.readNextChunk(chunk)) {
        std::cout << chunk << std::endl;
        std::cout << chunk.size() << std::endl;
    }*/
    //core.startSeacrhing();
    
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

    //try {
    //    //UnifiedReader reader("C://src/12.docx");
    //    //UnifiedReader reader("C://src/syn.txt");
    //    //UnifiedReader reader("C://src/UTF8.txt");
    //    UnifiedReader reader("C://src/12.pdf");
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