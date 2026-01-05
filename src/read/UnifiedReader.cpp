#include <read/UnifiedReader.hpp>
#include <filesystem>

reader_v openFile(const std::string& fileName) {
     std::filesystem::path p(fileName);

     std::string extension = p.extension().string();
     if (extension == ".pdf") {
         return MmapReader(fileName);
     }
     else if (extension == ".docx") {
         return DocxReader(fileName);
     }
     else {
         return MmapReader(fileName);
     }
}