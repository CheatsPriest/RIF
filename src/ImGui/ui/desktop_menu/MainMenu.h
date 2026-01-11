#pragma once

#include <ImGui/Window.hpp>
#include <nfd.hpp>
#include <configs/SearchConfig.hpp>
#include <unicode/unistr.h>
#include <string>
#include <array>

class MainMenu {
private:

    SearchConfig& config;

    string UTF8toUTF16(const std::string& utf8) {
        icu::UnicodeString unicodeStr = icu::UnicodeString::fromUTF8(utf8);
        string result;

        // Конвертируем UnicodeString в std::wstring
        result.resize(unicodeStr.length());
        for (int32_t i = 0; i < unicodeStr.length(); ++i) {
            result[i] = (char_t)unicodeStr[i];
        }
        return result;
    }

    // Функция для конвертации UTF-16 → UTF-8
    std::string UTF16toUTF8(const string& utf16) {
        icu::UnicodeString unicodeStr((const char_t*)utf16.c_str(), utf16.length());
        std::string result;
        unicodeStr.toUTF8String(result);
        return result;
    }
    std::string utf8Input;

public:
    MainMenu() : config(SearchConfig::get()){}

	void drawMenu() {

        ImGui::Begin("Управление");

        ImGui::InputText("Ввод", &utf8Input);
        
        if (ImGui::Button("Нажми меня")) {
            //counter++;
        }
        ImGui::SameLine();
        //ImGui::Text("Счетчик: %d", counter);

        ImGui::Separator();
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);


        if (ImGui::Button("Выбрать папку")) {
            NFD::UniquePath outPath;

            // Вызов диалога выбора папки
            nfdresult_t result = NFD::PickFolder(outPath);

            if (result == NFD_OKAY) {
                std::cout << "Выбрана папка: " << outPath.get() << std::endl;
                // Сохраните путь в свою переменную
            }
            else if (result == NFD_CANCEL) {
                std::cout << "Выбор отменен" << std::endl;
            }
            else {
                std::cout << "Ошибка: " << NFD::GetError() << std::endl;
            }
        }

        ImGui::End();
	}
};