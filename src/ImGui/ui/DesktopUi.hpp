#pragma once

#include <ImGui/Window.hpp>
#include <nfd.hpp>
#include <configs/SearchConfig.hpp>

class DesktopUi {
private:
    Window app;


public:
    DesktopUi() : app(1280, 720, "RIF") {
    }
    void run() {
        float color[3] = { 0.1f, 0.1f, 0.1f };
        int counter = 0;
        while (!app.shouldClose()) {
            app.newFrame();

            ImGui::Begin("Управление");

            ImGui::Text("Привет! Это твое окно на OpenGL + ImGui.");
            ImGui::ColorEdit3("Цвет фона", color); // Слайдер цвета

            if (ImGui::Button("Нажми меня")) {
                counter++;
            }
            ImGui::SameLine();
            ImGui::Text("Счетчик: %d", counter);

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

            app.endFrame();
        }
    }
    ~DesktopUi() {
    }
};

