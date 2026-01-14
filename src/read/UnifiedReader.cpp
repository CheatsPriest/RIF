#include <read/UnifiedReader.hpp>
#include <filesystem>
#include <format>

reader_v openFile(const std::string& fileName) {
     std::filesystem::path p(fileName);

     std::string extension = p.extension().string();
     std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

     if (extension == ".pdf") {
         return PdfReader(fileName);
     }
     else if (extension == ".docx") {
         return DocxReader(fileName);
     }
     else {
         return FileStreamReader(fileName);
     }
}