#include "FoldersInspector.hpp"

#include <iostream>
#include <filesystem>
#include <string>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <char.hpp>
// Мое
#include <global/GlobalQueues.hpp>
#include <ICU/Decoders.hpp>

namespace fs = std::filesystem;

class Memorizer {
private:
    std::unordered_set<std::string> set;
    std::unordered_set<fs::path> path_set;
public:
    bool testAndRemember(const std::string& str) {
        if (set.contains(str))return false;
        set.insert(str);
        return true;
    }bool testAndRemember(const fs::path& str) {
        if (path_set.contains(str))return false;
        path_set.insert(str);
        return true;
    }
    void clear() {
        set.clear();
        path_set.clear();
    }
};

struct info {
    fs::path folder;  
    unsigned cur_depth;
};


struct FoldersInspector::Impl {
    SearchConfig& config;
    SearchStats& stats;

    Lowercaser lowercase;
    void walk() {

        stats.reset();
        stats.is_inspecting_folders.store(true,std::memory_order::seq_cst);
        clearFolders();
        memory.clear();

        

        // Добавляем начальные папки
        for (const auto& folder : config.initial_folders) {

            
            if (fs::exists(folder) && fs::is_directory(folder)) {
                addFolder(folder, 0);
            }
            

        }

        while (!folders.empty() and stats.process_search.load(std::memory_order_acquire)) {
            info current = folders.front();
            folders.pop();
            

            processFolder(current.folder, current.cur_depth);
            
        }

        stats.is_inspecting_folders.store(false, std::memory_order::seq_cst);
        std::cout << "\nWalk finished.\n";
        stats.checkStatus();
    }

    Impl() : config(SearchConfig::get()), stats(SearchStats::get()), file_queue(FilesQueues::get()){

    }

private:
    Memorizer memory;
    std::queue<info> folders;
    file_queue_t& file_queue;
    void addFile(const fs::path& file) {
        if (!isExtensionAllowed(file))return;
        if (memory.testAndRemember(file)) {
            std::cout << "To process: " << file << std::endl;
            file_queue.push(file);
            std::cout << "Pushed" << std::endl;
            stats.files_to_process.fetch_add(1, std::memory_order_release);
        }
    }

    void addFolder(const fs::path& folder, unsigned depth) {
        if (memory.testAndRemember(folder)) {
            folders.push({ folder, depth });
        }
    }

    void clearFolders() {
        while (!folders.empty()) folders.pop();
    }

    bool processFolder(const fs::path& folder_path, unsigned current_depth) {
        try {
            for (const auto& entry : fs::directory_iterator(folder_path)) {
                if (!(stats.process_search.load(std::memory_order::seq_cst))) {
                    return false; 
                }

                const auto& path = entry.path();
                if (entry.is_directory()) {
                    // Проверяем, не игнорируется ли папка
                    if (shouldIgnoreFolder(path.filename().string())) {
                        continue;
                    }

                    // Проверяем глубину рекурсии
                    if (current_depth < config.depth) {
                        addFolder(path, current_depth + 1);
                    }
                }
                else if (entry.is_regular_file()) {
                    addFile(path);
                }
            }
            return true;
        }
        catch (const fs::filesystem_error& e) {
            std::cout << "Ошибка доступа к папке " << folder_path
                << ": " << e.what() << std::endl;
            return false;
        }
    }

    bool shouldIgnoreFolder(const std::string& folder_name) const {
        // Проверяем по имени папки
        if (config.ignored_folders.contains(folder_name)) {
            return true;
        }

        // Игнорируем скрытые папки (начинающиеся с . кроме ..)
        if (folder_name.empty()) return true;
        if (folder_name[0] == '.' && folder_name != "..") {
            return true;
        }

        return false;
    }

    bool isExtensionAllowed(const fs::path& file_path) const {
        if (config.allowed_extensions.empty()) {
            return true;
        }

        std::string ext = file_path.extension().string(); 
        if (ext.empty()) {
            return false; 
        }
        lowercase(ext);

        return config.allowed_extensions.contains(ext);
    }
};


FoldersInspector::FoldersInspector() : pImpl(std::make_unique<Impl>()) {}
FoldersInspector::~FoldersInspector() {

}

void FoldersInspector::walk() {
    pImpl->walk();
}