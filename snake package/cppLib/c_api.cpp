#include "ConsoleBackend.h" 
#include <vector>
#include <cstring>

#if defined(_WIN32)
    #define EXPORT_API __declspec(dllexport)
#else
    #define EXPORT_API __attribute__((visibility("default")))
#endif

using namespace conback;

// Простая POD-структура для проброса C-символов в ctypes
struct C_ConBackChar {
    wchar_t ch;
    uint8_t fgIdx; // uint4_t кастуется в uint8_t для проброса C-интерфейса
    uint8_t bgIdx;
};

extern "C" {

// ==================== ЖИЗНЕННЫЙ ЦИКЛ ====================

EXPORT_API ConsoleBackend* ConsoleBackend_Create() {
    return new (std::nothrow) ConsoleBackend();
}

EXPORT_API void ConsoleBackend_Destroy(ConsoleBackend* handle) {
    delete handle;
}

EXPORT_API void ConsoleBackend_Init(ConsoleBackend* handle) {
    if (handle) handle->init();

    handle->setPallete(CONBACK_DEFAULT_PALETTE);
}

EXPORT_API void ConsoleBackend_InitWithConfig(
    ConsoleBackend* handle, 
    int alignment, 
    const ColorRGB* palette16, 
    int style, 
    bool cursorVisibility
) {
    if (!handle) return;
    Config cfg;
    cfg.alignment = static_cast<Alignment>(alignment);
    cfg.style = static_cast<PixelStyle>(style);
    cfg.cursorVisibility = cursorVisibility;
    
    if (palette16) {
        for (int i = 0; i < 16; ++i) {
            cfg.pallete[i] = palette16[i];
        }
    }
    handle->init(cfg);

    if (palette16) {
        std::array<ColorRGB, 16> arr;
        for (int i = 0; i < 16; ++i) arr[i] = palette16[i];
        handle->setPallete(arr);
    }
}

EXPORT_API void ConsoleBackend_Shutdown(ConsoleBackend* handle) {
    if (handle) handle->shutdown();
}

EXPORT_API void ConsoleBackend_ClearScreen(ConsoleBackend* handle) {
    if (handle) handle->clearScreen();
}

// ==================== ВЫВОД (PRINT) ====================

EXPORT_API void ConsoleBackend_PrintStr(ConsoleBackend* handle, const char* str) {
    if (handle && str) handle->printStr(str);
}

EXPORT_API void ConsoleBackend_PrintStrAt(ConsoleBackend* handle, const char* str, int x, int y) {
    if (handle && str) handle->printStr(str, x, y);
}

EXPORT_API void ConsoleBackend_PrintStr16Colors(
    ConsoleBackend* handle, 
    const C_ConBackChar* chars, 
    size_t count
) {
    if (!handle || !chars) return;
    std::vector<ConBackChar> vec;
    vec.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        vec.push_back({chars[i].ch, static_cast<uint4_t>(chars[i].fgIdx), static_cast<uint4_t>(chars[i].bgIdx)});
    }
    handle->printStr16Colors(vec);
}

EXPORT_API void ConsoleBackend_PrintImgTrueColor(
    ConsoleBackend* handle, 
    const ColorRGB* pixels, 
    size_t count, 
    int width
) {
    if (!handle || !pixels) return;
    std::vector<ColorRGB> vec(pixels, pixels + count);
    handle->printImgTrueColor(vec, width);
}

EXPORT_API void ConsoleBackend_PrintImg16Colors(
    ConsoleBackend* handle, 
    const uint8_t* indices, 
    size_t count, 
    int width
) {
    if (!handle || !indices) return;
    std::vector<uint4_t> vec;
    vec.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        vec.push_back(static_cast<uint4_t>(indices[i]));
    }
    handle->printImg16Colors(vec, width);
}

// ==================== РАЗМЕР И ПОЗИЦИОНИРОВАНИЕ ====================

EXPORT_API void ConsoleBackend_GetTerminalSize(ConsoleBackend* handle, int* width, int* height) {
    if (!handle || !width || !height) return;
    handle->getTerminalSize(*width, *height);
}

EXPORT_API void ConsoleBackend_SetTerminalSize(ConsoleBackend* handle, int width, int height) {
    if (handle) handle->setTerminalSize(width, height);
}

EXPORT_API void ConsoleBackend_SetCursorPos(ConsoleBackend* handle, int x, int y) {
    if (handle) handle->setCursorPos(x, y);
}

EXPORT_API void ConsoleBackend_SetCursorVisibility(ConsoleBackend* handle, bool visibility) {
    if (handle) handle->setCursorVisibility(visibility);
}

// ==================== СЕТТЕРЫ ====================

EXPORT_API void ConsoleBackend_SetAlignment(ConsoleBackend* handle, int alignment) {
    if (handle) handle->setAlignment(static_cast<Alignment>(alignment));
}

EXPORT_API void ConsoleBackend_SetPalette(ConsoleBackend* handle, const ColorRGB* palette16) {
    if (!handle || !palette16) return;
    std::array<ColorRGB, 16> arr;
    for (int i = 0; i < 16; ++i) arr[i] = palette16[i];
    handle->setPallete(arr);
}

EXPORT_API void ConsoleBackend_SetFgColorIndex(ConsoleBackend* handle, uint8_t fgIdx) {
    if (handle) handle->setFgColorIndex(static_cast<uint4_t>(fgIdx));
}

EXPORT_API void ConsoleBackend_SetBgColorIndex(ConsoleBackend* handle, uint8_t bgIdx) {
    if (handle) handle->setBgColorIndex(static_cast<uint4_t>(bgIdx));
}

EXPORT_API void ConsoleBackend_SetPixelStyle(ConsoleBackend* handle, int style) {
    if (handle) handle->setPixelStyle(static_cast<PixelStyle>(style));
}

// ==================== ВВОД (INPUT) ====================

EXPORT_API void ConsoleBackend_UpdateInput(ConsoleBackend* handle) {
    if (handle) handle->updateInput();
}

EXPORT_API bool ConsoleBackend_IsKeyPressed(ConsoleBackend* handle, int key) {
    return handle ? handle->isKeyPressed(static_cast<Key>(key)) : false;
}

EXPORT_API bool ConsoleBackend_IsKeyReleased(ConsoleBackend* handle, int key) {
    return handle ? handle->isKeyReleased(static_cast<Key>(key)) : false;
}

EXPORT_API bool ConsoleBackend_IsKeyJustPressed(ConsoleBackend* handle, int key) {
    return handle ? handle->isKeyJustPressed(static_cast<Key>(key)) : false;
}

EXPORT_API bool ConsoleBackend_IsKeyJustReleased(ConsoleBackend* handle, int key) {
    return handle ? handle->isKeyJustReleased(static_cast<Key>(key)) : false;
}

} // extern "C"