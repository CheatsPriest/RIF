#pragma once
#include <variant>
#include <read/binary/BinaryReader.hpp>
#include <read/docx/DocxReader.hpp>
#include <read/pdf/PdfReader.hpp>
using reader_v = std::variant<FileStreamReader, DocxReader, PdfReader>;