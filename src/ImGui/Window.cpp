#include "Window.hpp"

Window::Window(int w, int h, const std::string& title)
    : m_width(w), m_height(h), m_title(title) {

    initGLFW();
    initImGui();
}

void Window::initGLFW() {
    if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");

    // Указываем версию OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Для macOS
#endif

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // Включаем V-Sync (чтобы не жарить GPU)
}

void Window::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;


    std::string fontPath = std::string(INTERNAL_PATH) + "/assets/fonts/NotoSans-Medium.ttf";
   
    ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());

    if (font == nullptr) {
        // Если шрифт не найден, ImGui по умолчанию использует встроенный (без кириллицы)
        // Можно вывести предупреждение в консоль
        printf("Warning: Font %s not found!\n", fontPath);
    }

    // Инициализация бэкендов
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 130"); // Стандартный шейдерный язык

    ImGui::StyleColorsDark();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void Window::newFrame() {
    glfwPollEvents(); // Обработка событий ОС (мышь, клава, ресайз)

    // Обновляем размеры для геттеров
    glfwGetFramebufferSize(m_window, &m_width, &m_height);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Window::endFrame() {
    ImGui::Render();

    // Очистка экрана (OpenGL)
    glViewport(0, 0, m_width, m_height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(m_window);
}

Window::~Window() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}
