#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOGDI
#define NOUSER
#define NOSOUND
#define NOMSG
#define NOMCX
#define NOIME
#define NOSERVICE
#define NOATOM
#define NOKANJI
#define NOKERNEL
#define NOSHLWAPI
#define NODRAWTEXT
#define NOMB
#define NOWH
#define NOCOMM
#define NOPROFILER
#define NOMINMAX
#define NCrypt
#define NOGDICAPMASKS
#define NOCTLMGR
#define NODEFERWINDOWPOS
#define NOHELP
#define NOMACROS
#define NOPROXYSTUB
#define NORPC
#define NOSCROLL
#define NOSHOWWINDOW
#define NOTEXTMETRIC
#define NOWINOFFSETS
#define NOCOLOR
#define NOHOTKEY
#define NODESKTOP
#define NOCLIPBOARD
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NORESOURCE
#define NOTAPE
#define NOWH
#define NOWINMESSAGES
#define NOWINOFFSETS
#define NOWINSTYLES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define NOATOM
#define NOCRYPT
#define NOMSG
#pragma once

// Минимальные заголовки Windows
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <string>
#include <stdexcept>
#include <iostream>

#include "char.hpp"  // Предполагаем, что тут: using char_t = char;

class MemoryMapWindows {
private:
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMap = NULL;
    const char* mappedData = nullptr;  // Сырые байты
    size_t fileSize = 0;

public:
    MemoryMapWindows(const std::string& filename) {
        if (!open(filename)) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
    }

    ~MemoryMapWindows() {
        close();
    }

    bool open(const std::string& filename) {
        close();

        hFile = CreateFileA(
            filename.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
            NULL
        );

        if (hFile == INVALID_HANDLE_VALUE) {
            return false;
        }

        LARGE_INTEGER liSize;
        if (!GetFileSizeEx(hFile, &liSize)) {
            CloseHandle(hFile);
            hFile = INVALID_HANDLE_VALUE;
            return false;
        }

        fileSize = static_cast<size_t>(liSize.QuadPart);

        // Пустой файл - нормально
        if (fileSize == 0) {
            CloseHandle(hFile);
            hFile = INVALID_HANDLE_VALUE;
            return true;
        }

        hMap = CreateFileMappingA(
            hFile,
            NULL,
            PAGE_READONLY,
            0, 0,  // Весь файл
            NULL
        );

        if (!hMap) {
            CloseHandle(hFile);
            hFile = INVALID_HANDLE_VALUE;
            return false;
        }

        mappedData = static_cast<const char*>(
            MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, fileSize)  // Указать размер!
            );

        if (!mappedData) {
            CloseHandle(hMap);
            hMap = NULL;
            CloseHandle(hFile);
            hFile = INVALID_HANDLE_VALUE;
            return false;
        }

        return true;
    }

    // Прочитать байт по позиции
    char readByteAt(size_t position) const {
        if (!mappedData || position >= fileSize) {
            return '\0';
        }
        return mappedData[position];
    }

    // Прочитать символ как char_t
    // Предполагаем, что char_t == char для UTF-8
    char_t readCharAt(size_t position) const {
        return static_cast<char_t>(readByteAt(position));
    }

    const char* rawData() const {
        return mappedData;
    }

    // Для совместимости с кодом, ожидающим char_t*
    const char_t* data() const {
        // Безопасный каст только если char_t == char
        static_assert(sizeof(char_t) == sizeof(char),
            "char_t must be same size as char");
        return reinterpret_cast<const char_t*>(mappedData);
    }

    const char_t* dataAt(size_t offset) const {
        if (!mappedData || offset >= fileSize) {
            return nullptr;
        }
        static_assert(sizeof(char_t) == sizeof(char),
            "char_t must be same size as char");
        return reinterpret_cast<const char_t*>(mappedData + offset);
    }

    size_t size() const {
        return fileSize;
    }

    bool isOpen() const {
        return mappedData != nullptr;
    }

    void close() {
        if (mappedData) {
            UnmapViewOfFile(mappedData);
            mappedData = nullptr;
        }
        if (hMap) {
            CloseHandle(hMap);
            hMap = NULL;
        }
        if (hFile != INVALID_HANDLE_VALUE) {
            CloseHandle(hFile);
            hFile = INVALID_HANDLE_VALUE;
        }
        fileSize = 0;
    }

    // Запретить копирование
    MemoryMapWindows(const MemoryMapWindows&) = delete;
    MemoryMapWindows& operator=(const MemoryMapWindows&) = delete;

    // Разрешить перемещение
    MemoryMapWindows(MemoryMapWindows&& other) noexcept {
        *this = std::move(other);
    }

    MemoryMapWindows& operator=(MemoryMapWindows&& other) noexcept {
        if (this != &other) {
            close();

            hFile = other.hFile;
            hMap = other.hMap;
            mappedData = other.mappedData;
            fileSize = other.fileSize;

            other.hFile = INVALID_HANDLE_VALUE;
            other.hMap = NULL;
            other.mappedData = nullptr;
            other.fileSize = 0;
        }
        return *this;
    }
};