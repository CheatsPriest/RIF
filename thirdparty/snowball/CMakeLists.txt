# thirdparty/snowball/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

project(snowball C)

# Список основных исходников
set(SNOWBALL_MAIN_SOURCES
    libstemmer/libstemmer.c
    runtime/api.c
    runtime/utilities.c
)

# Автоматически находим все файлы стеммеров
file(GLOB STEMMER_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src_c/*.c
)

# Объединяем все исходники
set(ALL_SOURCES ${SNOWBALL_MAIN_SOURCES} ${STEMMER_SOURCES})

# Создаем статическую библиотеку
add_library(snowball STATIC ${ALL_SOURCES})

# Подключаем заголовочные файлы
target_include_directories(snowball
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/runtime
)

# Определения для Windows/Visual Studio
if(MSVC)
    target_compile_definitions(snowball PRIVATE 
        HAVE_CONFIG_H
        _CRT_SECURE_NO_WARNINGS
        _LIB  # Для статической библиотеки
    )
    # Отключаем предупреждения для сторонней библиотеки
    target_compile_options(snowball PRIVATE 
        /W0  # Все предупреждения
        /wd4267  # size_t -> int conversion
        /wd4244  # conversion with possible loss of data
    )
else()
    target_compile_definitions(snowball PRIVATE HAVE_CONFIG_H)
endif()

# Свойства библиотеки
set_target_properties(snowball PROPERTIES
    ARCHIVE_OUTPUT_NAME snowball
    POSITION_INDEPENDENT_CODE ON
)

