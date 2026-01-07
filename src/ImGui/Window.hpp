#pragma once
#pragma once
#include <string>
#include <stdexcept>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h> // GLFW сам подключит нужные заголовки OpenGL

class Window {
public:
    Window(int w, int h, const std::string& title);
    ~Window();

    bool shouldClose() const;
    void newFrame();
    void endFrame();

    // Для удобства UI
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    GLFWwindow* m_window = nullptr;
    int m_width, m_height;
    std::string m_title;

    void initGLFW();
    void initImGui();
};
