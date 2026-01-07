#pragma once
#include <variant>
#include <read/binary/BinaryReader.hpp>
#include <read/docx/DocxReader.hpp>

using reader_v = std::variant<BinaryReader, DocxReader>;