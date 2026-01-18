#pragma once


#include <mupdf/fitz.h>
#include <stdio.h>

#include <stdlib.h>
#include <read/BaseReader.hpp>

class PdfReader : public BaseReader<PdfReader> {
private:
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
    PdfReader(const std::string& filename) : has_text(true), page_count(0), cur_page(0){

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

        buffer.reserve(CHUNK_SIZE + 1024);

    }
    bool readNextChunkImpl(std::u16string& chunk) {
        if (!has_text or cur_page>= page_count)return false;
        buffer.clear();
        
        for (; cur_page < page_count and buffer.size() < CHUNK_SIZE; ) {
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

                buffer += fz_string_from_buffer(ctx, buf);
            }
            fz_always(ctx) {
                fz_drop_output(ctx, out);
                fz_drop_buffer(ctx, buf);
                fz_drop_stext_page(ctx, text_page);
                fz_drop_page(ctx, page);
                cur_page++;
            }
            fz_catch(ctx) {
                
            }
        }
        chunk = to_utf16(buffer);
        
        return true;
    }
    PdfReader(PdfReader&& mv) noexcept : ctx(mv.ctx), doc(mv.doc), has_text(mv.has_text), page_count(mv.page_count), cur_page(mv.cur_page),
        buffer(std::move(mv.buffer)) {
        mv.ctx = nullptr;
        mv.doc = nullptr;
    }
    ~PdfReader() {
        if (ctx) {
            fz_drop_document(ctx, doc);
            fz_drop_context(ctx);
        }
    }
    void rewindImpl() {
        cur_page = 0;
    }
};