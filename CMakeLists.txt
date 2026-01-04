# CMakeList.txt: проект CMake для RIF; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.10)


project ("RIF")



set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Автоопределение платформы
if(WIN32)
    message(STATUS "Building for Windows")
    set(RIF_WINDOWS 1)
    
    # MSVC specific settings
    if(MSVC)
        if(POLICY CMP0141)
            cmake_policy(SET CMP0141 NEW)
            set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
        endif()
        
        # Подавить предупреждения для MSVC
        add_compile_options(/wd4267 /wd4018 /wd4715)
    endif()
    
elseif(UNIX AND NOT APPLE)
    message(STATUS "Building for Linux")
    set(RIF_LINUX 1)
endif()


if(RIF_WINDOWS)
    
    


    if(DEFINED ENV{BOOST_ROOT})
        set(BOOST_ROOT "$ENV{BOOST_ROOT}")
    else()
        
        if(EXISTS "C:/boost_1_89_0")
            set(BOOST_ROOT "C:/boost_1_89_0")
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/boost_1_89_0")
            set(BOOST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/boost_1_89_0")
        else()
           
            set(BOOST_ROOT "")
        endif()
    endif()
    
    if(BOOST_ROOT)
        set(Boost_DIR "${BOOST_ROOT}/lib/cmake/Boost-1.89.0")
        set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")
        set(BOOST_INCLUDEDIR "${BOOST_ROOT}")
    endif()
    
    find_package(Boost 1.89.0 REQUIRED)
endif()

find_package(Boost REQUIRED)


# Добавьте источник в исполняемый файл этого проекта.
add_executable (RIF "src/main.cpp" "src/configs/SearchConfig.hpp" "src/configs/SearchConfig.cpp" "src/global/GlobalQueues.hpp" "src/folders_inspector/FoldersInspector.cpp" "src/global/ThreadSafeQueue.hpp"
"src/read/BaseReader.hpp" "src/char.hpp" "src/read/mmap/MemoryMapWindows.h" 
"src/read/mmap/MmapReader.h" "src/read/mmap/MmapReader.cpp" "src/read/ReadersVariants.hpp"
"src/read/UnifiedReader.hpp" "src/read/UnifiedReader.cpp"
"src/search/ThreadSerachPool.hpp" "src/search/SearchExact.hpp"
"src/core/core.hpp" "src/core/Core.cpp"  "src/search/ThreadSerachPool.cpp"
"src/preproc/Preprocessor.hpp" "src/preproc/Preprocessor.cpp" "src/preproc/PreRegisters.hpp" "src/search/SearchEngine.hpp" "src/search/SearchRawResult.hpp" "src/read/docx/DocxReader.hpp" "src/read/docx/DocxReader.cpp")

# Платформо-специфичные файлы
if(SQLID_WINDOWS)
    #target_sources(SQLid PRIVATE "src/read/mmap/MemoryMapWindows.h")
elseif(SQLID_LINUX)
    target_sources(SQLid PRIVATE "src/read/mmap/MemoryMapLinux.h")
endif()

#target_include_directories(RIF PRIVATE ${Boost_INCLUDE_DIRS})

# Директории include
target_include_directories(RIF 
    PRIVATE src 
    PUBLIC include
    PUBLIC ${Boost_INCLUDE_DIRS}
)
# TODO: Добавьте тесты и целевые объекты, если это необходимо.

# ============================================================================
# DuckX Configuration (DOCX support) - Using pre-built library
# ============================================================================

if(RIF_WINDOWS)
    set(DUCKX_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/windows/DuckX")
    
    # Проверяем наличие исходников
    if(EXISTS "${DUCKX_SOURCE_DIR}/include/duckx.hpp")
        message(STATUS "Adding DuckX as subproject from: ${DUCKX_SOURCE_DIR}")
        
        # Добавляем DuckX как статическую библиотеку
        add_library(duckx STATIC
            "${DUCKX_SOURCE_DIR}/src/duckx.cpp"
            "${DUCKX_SOURCE_DIR}/thirdparty/pugixml/pugixml.cpp"
            "${DUCKX_SOURCE_DIR}/thirdparty/zip/zip.c"
        )
        
        # Подключаем заголовочные файлы
        target_include_directories(duckx PUBLIC
            "${DUCKX_SOURCE_DIR}/include"
            "${DUCKX_SOURCE_DIR}/thirdparty/pugixml"
            "${DUCKX_SOURCE_DIR}/thirdparty/zip"
        )
        
        # Подключаем к основному проекту
        target_include_directories(RIF PRIVATE "${DUCKX_SOURCE_DIR}/include")
        target_link_libraries(RIF duckx)
        
        # Подавляем предупреждения MSVC
        if(MSVC)
            target_compile_options(duckx PRIVATE /W0)
        endif()
        
        target_compile_definitions(RIF PRIVATE HAVE_DUCKX=1)
        
        message(STATUS "DOCX support enabled (built from source)")
    else()
        message(WARNING "DuckX sources not found. DOCX support disabled.")
        target_compile_definitions(RIF PRIVATE HAVE_DUCKX=0)
    endif()

endif()

set(SNOWBALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/snowball")
set(SNOWBALL_LIB_PATH "${SNOWBALL_DIR}/libstemmer.a")
set(SNOWBALL_INCLUDE_PATH "${SNOWBALL_DIR}/include")

# Проверяем наличие библиотеки
if(EXISTS "${SNOWBALL_LIB_PATH}")
    message(STATUS "Found Snowball library: ${SNOWBALL_LIB_PATH}")
    
    # Добавляем заголовочные файлы
    target_include_directories(RIF PRIVATE "${SNOWBALL_INCLUDE_PATH}")
    
    # Линкуем библиотеку
    target_link_libraries(RIF "${SNOWBALL_LIB_PATH}")
    
    # Для Windows может потребоваться явно указать линковку C библиотеки
    if(MSVC)
        # Создаем импортированную цель
        add_library(snowball STATIC IMPORTED)
        set_target_properties(snowball PROPERTIES
            IMPORTED_LOCATION "${SNOWBALL_LIB_PATH}"
        )
        target_link_libraries(RIF snowball)
    endif()
    
    target_compile_definitions(RIF PRIVATE HAVE_SNOWBALL=1)
    
else()
    message(WARNING "Snowball library not found at: ${SNOWBALL_LIB_PATH}")
    message(WARNING "Stemming will be disabled")
    target_compile_definitions(RIF PRIVATE HAVE_SNOWBALL=0)
endif()