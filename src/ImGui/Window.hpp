#pragma once
#pragma once
#include <string>
#include <stdexcept>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>
#include <GLFW/glfw3.h> // GLFW сам подключит нужные заголовки OpenGL
#include <unicode/unistr.h>

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

// UTF-8 → UTF-16
static std::u16string utf8_to_utf16_icu(const std::string& utf8_str) {
    icu::UnicodeString unicode_str = icu::UnicodeString::fromUTF8(utf8_str);
    std::u16string utf16_str;

    int32_t length = unicode_str.length();
    utf16_str.resize(length);

    for (int32_t i = 0; i < length; ++i) {
        utf16_str[i] = (char16_t)unicode_str[i];
    }

    return utf16_str;
}

// UTF-16 → UTF-8
static std::string utf16_to_utf8_icu(const std::u16string& utf16_str) {
    // Создаем UnicodeString из UTF-16 данных
    icu::UnicodeString unicode_str(
        reinterpret_cast<const UChar*>(utf16_str.data()),
        static_cast<int32_t>(utf16_str.length())
    );

    // Конвертируем в UTF-8 строку
    std::string utf8_str;
    unicode_str.toUTF8String(utf8_str);

    return utf8_str;
}