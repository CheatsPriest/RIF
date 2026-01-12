cmake_minimum_required(VERSION 3.15) # Рекомендуется 3.15+ для нормальной работы с ICU/Boost
project(RIF VERSION 1.0)

# 1. Настройки стандартов
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Поиск зависимостей (через vcpkg тулчейн)
find_package(ICU REQUIRED COMPONENTS uc i18n)
find_package(Boost REQUIRED) 
find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(nfd CONFIG REQUIRED)

# 3. Политики и настройки компилятора
if(MSVC)
    add_compile_options(/wd4267 /wd4018 /wd4715 /utf-8) # utf-8 важен для ICU
    if(POLICY CMP0141)
        cmake_policy(SET CMP0141 NEW)
    endif()
endif()

# 4. Список файлов (для удобства поддержки)
find_path(IMGUI_BACKENDS_DIR NAMES imgui_impl_glfw.cpp PATH_SUFFIXES backends imgui/backends)
add_executable (RIF "src/main.cpp" "src/configs/SearchConfig.hpp" 
"src/configs/SearchConfig.cpp" "src/global/GlobalQueues.hpp" "src/folders_inspector/FoldersInspector.cpp" "src/global/ThreadSafeQueue.hpp"
"src/read/BaseReader.hpp" "src/char.hpp" "src/read/ReadersVariants.hpp"
"src/read/UnifiedReader.hpp" "src/read/UnifiedReader.cpp"
"src/search/ThreadSerachPool.hpp" "src/search/SearchExact.hpp"
"src/core/core.hpp" "src/core/Core.cpp"  "src/search/ThreadSerachPool.cpp"
"src/preproc/Preprocessor.hpp" "src/preproc/Preprocessor.cpp" "src/preproc/PreRegisters.hpp" 
"src/search/SearchEngine.hpp" "src/search/SearchRawResult.hpp" "src/read/docx/DocxReader.hpp" 
"src/read/docx/DocxReader.cpp" "src/char.cpp" "src/search/synonymous/Stemmer.hpp" 
"src/search/synonymous/Stemmer.cpp" "src/preproc/PreSynonyms.hpp" "src/preproc/PreSynonyms.cpp" 
"src/search/synonymous/StemmerPipeline.hpp" "src/search/synonymous/SteammerPipeline.cpp" "src/search/SearchSynonymous.hpp" 
"src/search/SearchEngine.cpp" "src/read/binary/FileStreamReader.hpp" "src/read/binary/BinaryReader.hpp" 
"src/ImGui/Window.hpp" "src/ImGui/Window.cpp"
 "src/ImGui/ui/DesktopUi.hpp" "src/search/caching/LRUSteamming.hpp" "src/aho_corasick/SymbolsTree.hpp" "src/aho_corasick/TreeWalker.hpp" "src/ImGui/ui/desktop_menu/MainMenu.h" "src/ImGui/ui/desktop_menu/MainMenu.cpp" "src/ICU/Decoders.hpp" "src/ICU/Decoders.cpp" "src/results/SearchResult.hpp" "src/results/SearchResult.cpp")

target_compile_definitions(RIF PRIVATE 
    PROJECT_PATH="${CMAKE_CURRENT_SOURCE_DIR}"
)

# 6. Подключение инклюдов (Modern CMake использует target_...)
find_path(IMGUI_INCLUDE_DIR NAMES imgui.h)

target_include_directories(RIF PRIVATE 
    src 
    ${Boost_INCLUDE_DIRS}
    ${IMGUI_INCLUDE_DIR}           # Путь к imgui.h
    ${IMGUI_INCLUDE_DIR}/backends  # Путь к бэкендам (чтобы работало #include "imgui_impl_glfw.h")
)

# 7. Конфигурация DuckX (DOCX)
set(DUCKX_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/windows/DuckX")
if(EXISTS "${DUCKX_SOURCE_DIR}/include/duckx.hpp")
    add_library(duckx STATIC
        "${DUCKX_SOURCE_DIR}/src/duckx.cpp"
        "${DUCKX_SOURCE_DIR}/thirdparty/pugixml/pugixml.cpp"
        "${DUCKX_SOURCE_DIR}/thirdparty/zip/zip.c"
    )
    target_include_directories(duckx PUBLIC 
        "${DUCKX_SOURCE_DIR}/include"
        "${DUCKX_SOURCE_DIR}/thirdparty/pugixml"
        "${DUCKX_SOURCE_DIR}/thirdparty/zip"
    )
    target_link_libraries(RIF PRIVATE duckx)
    target_compile_definitions(RIF PRIVATE HAVE_DUCKX=1)
    if(MSVC)
        target_compile_options(duckx PRIVATE /W0)
    endif()
endif()

# 8. Конфигурация Snowball (Stemmer)
set(SNOWBALL_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/snowball/libstemmer.a")
if(EXISTS "${SNOWBALL_LIB_PATH}")
    target_include_directories(RIF PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/snowball/include")
    target_link_libraries(RIF PRIVATE "${SNOWBALL_LIB_PATH}")
    target_compile_definitions(RIF PRIVATE HAVE_SNOWBALL=1)
else()
    target_compile_definitions(RIF PRIVATE HAVE_SNOWBALL=0)
endif()

# 9. Финальная линковка библиотек (Без ошибок и //)
target_link_libraries(RIF PRIVATE 
    ICU::uc 
    ICU::i18n
    Boost::boost
    imgui::imgui
    glfw             
    opengl32
    nfd::nfd
)