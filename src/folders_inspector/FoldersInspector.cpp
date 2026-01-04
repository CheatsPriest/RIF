#include "FoldersInspector.hpp"

#include <iostream>
#include <filesystem>
#include <string>
#include <unordered_set>
#include <queue>
#include <algorithm>

// Мое
#include <global/GlobalQueues.hpp>

namespace fs = std::filesystem;

class Memorizer {
private:
    std::unordered_set<std::string> set;

public:
    bool testAndRemember(const std::string& str) {
        if (set.contains(str))return false;
        set.insert(str);
        return true;
    }
    void clear() {
        set.clear();
    }
};

struct info {
    std::string folder;
    unsigned cur_depth;
};

struct FoldersInspector::Impl {
    SearchConfig& config;

    void walk() {

        clearFolders();
        memory.clear();

        // Добавляем начальные папки
        for (const auto& folder : config.initial_folders) {
            if (fs::exists(folder) && fs::is_directory(folder)) {
                addFolder(folder, 0);
            }
        }

        while (!folders.empty() and config.process_search.load(std::memory_order_acquire)) {
            info current = folders.front();
            folders.pop();

            processFolder(current.folder, current.cur_depth);
        }

        std::cout << "\nОбход завершён.\n";
    }

    Impl() : config(SearchConfig::get()) {

    }

private:
    Memorizer memory;
    std::queue<info> folders;

    void addFile(std::string&& file) {
        if (!isExtensionAllowed(file))return;
        if (memory.testAndRemember(file)) {
            std::cout << "To process: " << file << std::endl;
            FilesQueues::get().push(std::move(file));
        }
    }

    void addFolder(const std::string& folder, unsigned depth) {
        if (memory.testAndRemember(folder)) {
            folders.push({ folder, depth });
        }
    }

    void clearFolders() {
        while (!folders.empty()) folders.pop();
    }

    bool processFolder(const std::string& folder_path, unsigned current_depth) {
        try {
            for (const auto& entry : fs::directory_iterator(folder_path)) {
                if (!config.process_search.load(std::memory_order_acquire)) {
                    return false; 
                }

                const auto& path = entry.path();
                std::string path_str = path.string();

                if (entry.is_directory()) {
                    // Проверяем, не игнорируется ли папка
                    if (shouldIgnoreFolder(path.filename().string())) {
                        continue;
                    }

                    // Проверяем глубину рекурсии
                    if (current_depth < config.depth) {
                        addFolder(path_str, current_depth + 1);
                    }
                }
                else if (entry.is_regular_file()) {
                    addFile(std::move(path_str));
                }
            }
            return true;
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "Ошибка доступа к папке " << folder_path
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

    bool isExtensionAllowed(const std::string& file_path) const {
        if (config.allowed_extensions.empty()) {
            return true; 
        }

        size_t dot_pos = file_path.find_last_of('.');
        if (dot_pos == std::string::npos) {
            return false; 
        }

        std::string ext = file_path.substr(dot_pos);

        
        {
            std::string ext_lower = ext;
            std::transform(ext_lower.begin(), ext_lower.end(),
                ext_lower.begin(), ::tolower);
            if (config.allowed_extensions.contains(ext_lower))return true;
        }

        return false;
    }
};


FoldersInspector::FoldersInspector() : pImpl(std::make_unique<Impl>()) {}
FoldersInspector::~FoldersInspector() = default;

void FoldersInspector::walk() {
    pImpl->walk();
}