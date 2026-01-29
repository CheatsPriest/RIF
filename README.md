# RIF - Rapid Info Finder
Поисковый движок по локальным файлам(продвинутый аналог grep)

## Идея
Искать подстроки текста разными способами за O(n) без предварительной индексации. Особенности:
- 2 режима, точный поиск и поиск по синонимам
- Поддержка wildcards в точном поиске
- Поддержка синонимического поиска при помощи морфологического анализа и дерева синонимов с сохранением O(n) сложности поиска
- ICU для независимости от кодировки входного файла
- Поддержка текстовых документов, docx, pdf и оптического распознавания текста для pdf

## Использование
1. Скачать архив RIF.rar последнего релиза
2. Разархивировать его в C:/
3. Запустить RIF.exe в C:/RIF/RIF.exe

## Технические детали
- Язык: C++23
- std::generator<T> для ленивого чтения docx файлов
- Алгоритмы для умного поиска
- Многопоточность

## Производительность 
#### Таблица 1: Обычный поиск (Exact Match + Wildcards)
```
| Потоки | Время (мс) | Скорость (МБ/с) | Ускорение (Scaling) |
|--------|------------|-----------------|---------------------|
| 1      | 315 мс     | 107 МБ/с        | 1.0x                |
| 5      | 64 мс      | 528 МБ/с        | 4.9x                |
| 10     | 45 мс      | 751 МБ/с        | 7x                  |
```

#### Таблица 2: Поиск с синонимами + обычный поиск (Synonyms + Stemming)
```
| Потоки | Время (мс) | Скорость (МБ/с) | Ускорение (Scaling) |
|--------|------------|-----------------|---------------------|
| 1      | 671 мс     | 50 МБ/с         | 1.0x                |
| 5      | 160 мс     | 211 МБ/с        | 4.2x                |
| 10     | 119 мс     | 284 МБ/с        | 5.6x                |
```

### 📚 Third-party Libraries & Contributors

**Core & System**
* [ICU](https://github.com) — International Components for Unicode.
* [ZLIB](https://github.com) — A mass-appeal data compression library.

**Graphical User Interface (GUI)**
* [Dear ImGui](https://github.com) — Bloat-free Graphical User interface for C++.
* [GLFW](https://github.com) — A multi-platform library for OpenGL, Window and Input.
* [Native File Dialog (NFD)](https://github.com) — A tiny cross-platform native file dialog library.
* [FreeType](https://github.com) — A freely available software library to render fonts.
* [HarfBuzz](https://github.com) — A software library for shaping Unicode text.

**OCR & Document Processing**
* [Tesseract OCR](https://github.com) — Open source Optical Character Recognition (OCR) Engine.
* [Leptonica](https://github.com) — An open source library for image processing and image analysis applications.
* [MuPDF](https://github.com) — A lightweight PDF, XPS, and E-book viewer.
* [Gumbo Parser](https://github.com) — An HTML5 parsing library in pure C.

**Image Formats**
* [OpenJPEG](https://github.com) — An open-source JPEG 2000 codec.
* [libjpeg-turbo](https://github.com) — A JPEG image codec that uses SIMD instructions to accelerate baseline JPEG compression and decompression.
* [jbig2dec](https://github.com) — A decoder library for the JBIG2 image compression format.
