#include <ImGui/ui/desktop_menu/MainMenu.h>
#include <format>


void MainMenu::optionsMenu()
{
    ImGui::Begin("Управление");

    ImGui::InputText("Поиск...", &search_input);

    ImGui::Checkbox("Учитывать регистр?", &config.respect_registers);
    ImGui::Checkbox("Учитывать синонимы?", &syn_setting.use_synonyms);

    ImGui::SliderInt("Глубина поиска файлов", &config.depth, 0, 250);

    ImGui::SliderInt("Прогрузка контекста слева в символах", &config.left_context, 0, 500);
    ImGui::SliderInt("Прогрузка контекста справа в символах", &config.right_context, 0, 500);

    resizeThreadPool();
    foldersChose();

    startSearch();

    ImGui::Text("Последний поиск занял %dмс", std::chrono::duration_cast<std::chrono::milliseconds>(end_point-start_point).count());

    ImGui::End();
}

void MainMenu::duringTheSearch()
{
    end_point = std::chrono::steady_clock::now();
    ImGui::Begin("Информатор поиска");
    ImGui::Text("Идет поиск, ожидайте");


    
    drawProgressSearchBar();

    if (ImGui::Button("Прервать")) {
        async_core.abortSearching();
    }
    
    ImGui::End();
}

void MainMenu::drawProgressSearchBar()
{
    int processed = stats.files_processed.load(std::memory_order_relaxed);
    int total = stats.files_to_process.load(std::memory_order_relaxed);

    float progress = (total > 0) ? (float)processed / (float)total : 0.0f;
    
    char overlay_buf[64];
    sprintf(overlay_buf, "Processed: %d / %d", processed, total);

    ImGui::Text("Search progress:");
    ImGui::ProgressBar(progress, ImVec2(-1.0f, 0.0f), overlay_buf);
}

void MainMenu::draw() {
    
    if (!stats.process_search.load(std::memory_order::acquire)) {
        optionsMenu();
    }
    else {
        duringTheSearch();
    }

}

void MainMenu::foldersChose() {
    if (ImGui::Button("Выбрать папку")) {
        NFD::UniquePath outPath;

        nfdresult_t result = NFD::PickFolder(outPath);

        if (result == NFD_OKAY) {
            std::string path = outPath.get();
            auto it = std::find(inital_folders.begin(), inital_folders.end(), path);
            if(it== inital_folders.end())inital_folders.push_back(std::move(path));
            
        }
        else if (result == NFD_CANCEL) {
            //std::cout << "Выбор отменен" << std::endl;
        }
        else {
            //std::cout << "Ошибка: " << NFD::GetError() << std::endl;
        }
    }
    ImGui::Text("Выбранные папки:");
    int id = 0;
    for (auto it = inital_folders.begin(); it != inital_folders.end(); ) {
        ImGui::PushID(id);
        ImGui::Text(it->c_str());
        ImGui::SameLine();
        if (ImGui::Button("Удалить")) {
            it = inital_folders.erase(it);
        }
        else {
            it++;
        }
        ImGui::PopID();
        id++;
    }
    if (ImGui::Button("Очистить все")) {
        inital_folders.clear();
    }
}

void MainMenu::startSearch()
{
    if (search_input.size()>0 and ImGui::Button("Начать")) {
        // Сама строка
        config.raw_templ = utf8_to_utf16_icu(search_input);

        // Настройки

        // Папки 
        config.initial_folders.clear();
        config.initial_folders.reserve(inital_folders.size());
        std::copy(inital_folders.begin(), inital_folders.end(), std::back_inserter(config.initial_folders));

        // Запуск(пинок ядру)
        async_core.startSeacrhing();
        start_point = std::chrono::steady_clock().now();
    }
}

void MainMenu::resizeThreadPool()
{
    static int optimal_threads = std::thread::hardware_concurrency();
    ImGui::SliderInt("Потоков на поиск", &config.amount_of_search_threads, 1, optimal_threads);
    ImGui::SameLine();
    if (ImGui::Button("Применить изменения")) {
        async_core.resizeSearchPool();
    }
}


