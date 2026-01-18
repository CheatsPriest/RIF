#include <read/UnifiedReader.hpp>
#include <filesystem>
#include <format>
#include <ICU/Decoders.hpp>
#include <ocr/OcrManager.hpp>
#include <configs/SearchConfig.hpp>



reader_v openFile(const std::filesystem::path& fileName) {
    

     std::filesystem::path p(fileName);

     std::string extension = p.extension().string();
     std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

     if (extension == ".pdf") {
         auto u8str = fileName.u8string();
         auto str = normalizeU8ToStd(u8str);
         
         static SearchConfig& config = SearchConfig::get();
         if (config.use_ocr_for_pdf) {
             static OcrManager& manager_link = OcrManager::get();
             manager_link.push(fileName);
         }
         
         return PdfReader(str);
     }
     else if (extension == ".docx") {
         return DocxReader(fileName.string());
     }
     else {
         return FileStreamReader(fileName.string());
     }
}