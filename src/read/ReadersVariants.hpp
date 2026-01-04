#pragma once
#include <variant>
#include <read/mmap/MmapReader.h>
#include <read/docx/DocxReader.hpp>

using reader_v = std::variant<MmapReader, DocxReader>;