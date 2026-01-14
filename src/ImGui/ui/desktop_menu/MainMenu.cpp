#include <ImGui/ui/desktop_menu/MainMenu.h>
#include <format>

#include <filesystem>

namespace fs = std::filesystem;

void MainMenu::optionsMenu()
{
    ImGui::Begin("Управление");

    ImGui::InputText("Поиск...", &search_input);

    ImGui::Checkbox("Учитывать регистр?", &config.respect_registers);
    ImGui::Checkbox("Искать по смыслу слов?", &syn_setting.use_synonyms);

    ImGui::SliderInt("Глубина поиска файлов", &config.depth, 0, 250);

    ImGui::SliderInt("Прогрузка контекста слева в символах", &config.left_context, 0, 500);
    ImGui::SliderInt("Прогрузка контекста справа в символах", &config.right_context, 0, 500);

    resizeThreadPool();
    foldersChose();

    startSearch();

    ImGui::Text("Последний поиск занял %dмс", std::chrono::duration_cast<std::chrono::milliseconds>(end_point-start_point).count());

    auto memory_read = stats.kbytes_read.load(std::memory_order_acquire);
    if(memory_read<1024)ImGui::Text("Прочитано %dкб", memory_read);
    else {
        memory_read /= 1024;
        if (memory_read < 1024)ImGui::Text("Прочитано %dмб", memory_read);
        else {
            memory_read /= 1024;
            ImGui::Text("Прочитано %dгб", memory_read);
        }
    }
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
    drawResultWindow();
}

void MainMenu::foldersChose() {
    if (ImGui::Button("Выбрать папку")) {
        NFD::UniquePath outPath;

        nfdresult_t result = NFD::PickFolder(outPath);
        
        if (result == NFD_OKAY) {
            std::string path = outPath.get();
            auto u16path = utf8_to_utf16_icu(path);
            fs::path pa(u16path);

            std::cout << pa.string() << std::endl;

            auto it = std::find(inital_folders.begin(), inital_folders.end(), pa);
            if(it== inital_folders.end())inital_folders.push_back(std::move(pa));
            
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
        ImGui::Text(utf16_to_utf8_icu(it->u16string()).c_str());
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


void MainMenu::loadNewResults()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    while (!result_queue.empty()) {
        loaded_results.push_back(result_queue.unwait_pop());
        std::this_thread::sleep_for(std::chrono::milliseconds(7));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void TextWindowWithCopy(const char* title, const std::string& content,
    float width = 0.0f, float height = 0.0f) {

    // Определяем размеры
    ImVec2 window_size(width, height);
    if (width <= 0) window_size.x = ImGui::GetIO().DisplaySize.x * 0.8f;
    if (height <= 0) window_size.y = ImGui::GetIO().DisplaySize.y * 0.6f;

    ImGui::Begin(title, nullptr,
        ImGuiWindowFlags_HorizontalScrollbar |
        ImGuiWindowFlags_AlwaysVerticalScrollbar);

    // Кнопка копирования в буфер обмена
    if (ImGui::Button("Копировать")) {
        ImGui::SetClipboardText(content.c_str());
    }

    ImGui::SameLine();
    ImGui::Text("(%zu символов)", content.size());

    ImGui::Separator();

    // Текст с авто-переносом
    ImGui::PushTextWrapPos(0.0f); // 0 = до края окна

    // Можно подсветка синтаксиса для кода
   
    ImGui::TextWrapped("%s", content.c_str());
    

    ImGui::PopTextWrapPos();

    ImGui::End();
}

void MainMenu::drawResultWindow()
{
    ImGui::Begin("Результаты");
    if (ImGui::Button("Очистить")) {
        loaded_results.clear();
    }

    loadNewResults();
    int id1 = 0, id2 = 1;
    for (auto& el : loaded_results) {
        ImGui::PushID(id1);
        ImGui::SeparatorText(el.file.c_str());
        ImGui::PopID();
        id1++;
        for (auto& place : el.places) {
            ImGui::PushID(id2);

            ImGui::Text("На позициях от %d до %d", place.left, place.right);

            ImGui::SameLine();
            if(ImGui::Button("Прогрузить контекст")){
                el.loadContextFor(place);
            }
            
            if (!place.context.empty()) {
                ImGui::SameLine();
                if (ImGui::Button("Показать контекст")) {
                    place.showMe = !place.showMe;
                }
            }
            if (place.showMe) {
                //ImGui::TextWrapped(place.context.c_str());
                TextWindowWithCopy(el.file.c_str(), place.context);
            }

            ImGui::PopID();
            id2++;
            ImGui::Separator();
        }

    }

    ImGui::End();
}
