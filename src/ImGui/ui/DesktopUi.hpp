#pragma once

#include <ImGui/Window.hpp>
#include <nfd.hpp>
#include <configs/SearchConfig.hpp>
#include <ImGui/ui/desktop_menu/MainMenu.h>
class DesktopUi {
private:
    Window app;

    MainMenu menu;

public:
    DesktopUi() : app(1280, 720, "RIF") {
    }
    void run() {
        float color[3] = { 0.1f, 0.1f, 0.1f };
        int counter = 0;
        while (!app.shouldClose()) {
            app.newFrame();

            menu.draw();

            app.endFrame();
        }
    }
    ~DesktopUi() {
    }
};

