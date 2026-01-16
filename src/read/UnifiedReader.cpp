#include <read/UnifiedReader.hpp>
#include <filesystem>
#include <format>
#include <ICU/Decoders.hpp>

reader_v openFile(const std::filesystem::path& fileName) {
     std::filesystem::path p(fileName);

     std::string extension = p.extension().string();
     std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

     if (extension == ".pdf") {
         auto u8str = fileName.u8string();
         auto str = normalizeU8ToStd(u8str);
         return PdfReader(str);
     }
     else if (extension == ".docx") {
         return DocxReader(fileName.string());
     }
     else {
         return FileStreamReader(fileName.string());
     }
}