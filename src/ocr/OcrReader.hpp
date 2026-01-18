#pragma once

#include <mupdf/fitz.h>
#include <stdio.h>

#include <stdlib.h>
#include <read/BaseReader.hpp>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

class OcrReader {
private:

    std::unique_ptr<tesseract::TessBaseAPI> ocr_api_ptr;

    fz_context* ctx = nullptr;
    fz_document* doc = nullptr;
    bool has_text;
    int page_count;
    int cur_page;

    std::string buffer;
    static std::u16string to_utf16(const std::string& utf8_str) {
        icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(utf8_str);
        return std::u16string(reinterpret_cast<const char16_t*>(ustr.getBuffer()), ustr.length());
    }
public:
    OcrReader(const std::string& filename) : has_text(true), page_count(0), cur_page(0) {

        
        


        ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
        if (!ctx) {
            std::cerr << "Cannot create mupdf context" << std::endl;
            has_text = false;
        }
        fz_register_document_handlers(ctx);
        fz_try(ctx) {
            doc = fz_open_document(ctx, filename.c_str());
            page_count = fz_count_pages(ctx, doc);
        }
        fz_catch(ctx) {
            has_text = false;
        }

        buffer.reserve(OCR_CHUNK_SIZE + 1024);

        //OCR
        auto path = std::string(PROJECT_PATH) + "/assets/tessdata";
        // --- Инициализация Tesseract ---
        ocr_api_ptr = std::make_unique<tesseract::TessBaseAPI>();
        // NULL использует TESSDATA_PREFIX, "rus+eng" для двуязычного поиска
        if (ocr_api_ptr->Init(path.c_str(), "rus+eng")) {
            std::cerr << "OCR Init Failed" << std::endl;
            ocr_api_ptr.reset(nullptr);
        }
        else
            ocr_api_ptr->SetPageSegMode(tesseract::PSM_AUTO);

    }
    bool readNextChunk(std::string_view& chunk) {
        if (!has_text or cur_page >= page_count or !ocr_api_ptr) return false;
        buffer.clear();
        fz_purge_glyph_cache(ctx);
        // Цикл идет, пока есть страницы И пока мы не набрали достаточно текста
        for (; cur_page < page_count && buffer.size() < OCR_CHUNK_SIZE; ) {
            fz_page* page = nullptr;
            fz_stext_page* text_page = nullptr;
            fz_buffer* buf = nullptr;
            fz_output* out = nullptr;

            fz_try(ctx) {
                page = fz_load_page(ctx, doc, cur_page);
                text_page = fz_new_stext_page_from_page(ctx, page, NULL);
                buf = fz_new_buffer(ctx, 1024);
                out = fz_new_output_with_buffer(ctx, buf);

                fz_print_stext_page_as_text(ctx, out, text_page);
                fz_close_output(ctx, out);

                std::string page_text = fz_string_from_buffer(ctx, buf);

                // Если текста на слое нет (меньше 10 символов) — включаем OCR
                if (page_text.length() < 10) {
                    fz_matrix upscl = fz_scale(2, 2);
                    // Используем fz_device_rgb(ctx) аккуратно
                    fz_pixmap* pix = fz_new_pixmap_from_page(ctx, page, upscl, fz_device_rgb(ctx), 0);

                    if (pix) {
                        ocr_api_ptr->SetImage(pix->samples, pix->w, pix->h, pix->n, pix->w * pix->n);

                        char* ocr_text = ocr_api_ptr->GetUTF8Text();
                        if (ocr_text) {
                            buffer += ocr_text;
                            delete[] ocr_text; // Для Tesseract 5.x это корректно
                            
                        }

                        // КРИТИЧНО: Освобождаем внутренние буферы Tesseract после обработки страницы
                        ocr_api_ptr->Clear();

                        fz_drop_pixmap(ctx, pix);
                    }


                }
                else {
                    buffer += page_text;
                }

                buffer += "\n"; 
            }
            fz_always(ctx) {
                fz_drop_output(ctx, out);
                fz_drop_buffer(ctx, buf);
                fz_drop_stext_page(ctx, text_page);
                fz_drop_page(ctx, page);
                cur_page++; 
            }
            fz_catch(ctx) {
                cur_page++; 
            }
            //if (cur_page % 25) {
            //    ocr_api_ptr->End();
            //    ocr_api_ptr.reset();
            //    //OCR
            //    auto path = std::string(PROJECT_PATH) + "/assets/tessdata";
            //    // --- Инициализация Tesseract ---
            //    ocr_api_ptr = std::make_unique<tesseract::TessBaseAPI>();
            //    // NULL использует TESSDATA_PREFIX, "rus+eng" для двуязычного поиска
            //    if (ocr_api_ptr->Init(path.c_str(), "rus+eng")) {
            //        std::cerr << "OCR Init Failed" << std::endl;
            //        ocr_api_ptr.reset(nullptr);
            //    }
            //    else
            //        ocr_api_ptr->SetPageSegMode(tesseract::PSM_AUTO);
            //}
        }

        if (buffer.empty() && cur_page >= page_count) return false;

        //chunk = std::string_view(buffer.begin(), buffer.end());
        chunk = buffer;
        return true; // Возвращаем true, пока есть данные
    }
    
    OcrReader(OcrReader&& mv) noexcept : ctx(mv.ctx), doc(mv.doc), has_text(mv.has_text), page_count(mv.page_count), cur_page(mv.cur_page),
        buffer(std::move(mv.buffer)),
    ocr_api_ptr(std::move(mv.ocr_api_ptr)){
        mv.ctx = nullptr;
        mv.doc = nullptr;
        mv.ocr_api_ptr = nullptr;
    }
    ~OcrReader() {
        if (ctx) {
            fz_drop_document(ctx, doc);
            fz_drop_context(ctx);
        }
        if (ocr_api_ptr) {
            ocr_api_ptr->End();
        }
    }

};