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
#define NOMINMAX
#include <windows.h>

#include <ImGui/Window.hpp>
#include <nfd.hpp>

#include <chrono>

#include <search/caching/LRUSteamming.hpp>

int main() {

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // Требует #include <windows.h>
    std::setvbuf(stdout, nullptr, _IOFBF, 1000); // Чтобы вывод не тормозил
#endif

   
    
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
    //try {

    //    Window app(1280, 720, "ImGui Window");

    //    // Данные для демонстрации в интерфейсе
    //    float color[3] = { 0.1f, 0.1f, 0.1f };
    //    int counter = 0;
    //    NFD_Init();
    //    // 2. Главный цикл приложения
    //    while (!app.shouldClose()) {
    //        // Начало кадра (обработка событий и подготовка ImGui)
    //        app.newFrame();

    //        // --- Здесь пишем код интерфейса ImGui ---

    //        // Создаем простое окно ImGui
    //        ImGui::Begin("Управление");

    //        ImGui::Text("Привет! Это твое окно на OpenGL + ImGui.");
    //        ImGui::ColorEdit3("Цвет фона", color); // Слайдер цвета

    //        if (ImGui::Button("Нажми меня")) {
    //            counter++;
    //        }
    //        ImGui::SameLine();
    //        ImGui::Text("Счетчик: %d", counter);

    //        ImGui::Separator();
    //        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);


    //        if (ImGui::Button("Выбрать папку")) {
    //            NFD::UniquePath outPath;

    //            // Вызов диалога выбора папки
    //            nfdresult_t result = NFD::PickFolder(outPath);

    //            if (result == NFD_OKAY) {
    //                std::cout << "Выбрана папка: " << outPath.get() << std::endl;
    //                // Сохраните путь в свою переменную
    //            }
    //            else if (result == NFD_CANCEL) {
    //                std::cout << "Выбор отменен" << std::endl;
    //            }
    //            else {
    //                std::cout << "Ошибка: " << NFD::GetError() << std::endl;
    //            }
    //        }

    //        ImGui::End();
    //        // ---------------------------------------

    //        // Конец кадра (рендер и вывод на экран)
    //        app.endFrame();
    //    }
    //}
    //catch (const std::exception& e) {
    //    // Обработка ошибок инициализации
    //    printf("Ошибка: %s\n", e.what());
    //    return -1;
    //}
    //NFD_Quit();
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
    
    SynonymsSettings::get().use_synonyms = false;

    Core core;
    int prob;

    SearchConfig::get().amount_of_search_threads = 5;
    core.resizeSearchPool();
    
    for (int i = 0; i < 1; i++) {
        auto start = std::chrono::steady_clock::now();
        core.startSeacrhing();

        while (stats.process_search);
        auto end = std::chrono::steady_clock::now();


        std::cout << "Completed Search: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
    }
   
    std::cin >> prob;

    cfg.raw_templ = u"деньги";
    
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
    //    UnifiedReader reader("C://src/big.txt");
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