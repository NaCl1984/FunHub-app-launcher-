#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include <cstdint>
#include "uint4_t.h"
#include <algorithm>

#ifdef _WIN32
    
    #include <windows.h>

#elif __linux__

    #include <termios.h>
    #include <unistd.h>

#endif

namespace conback{

enum class Alignment{
    LeftTop, CenterTop, RightTop,
    LeftCenter, Center, RightCenter,
    LeftBottom, CenterBottom, RightBottom 
};

struct ColorRGB{
    uint8_t r, g, b;

    ColorRGB(){ r = 0; g = 0; b = 0;}

    ColorRGB(uint8_t r, uint8_t g, uint8_t b): r(r), g(g), b(b) {}
};

enum class PixelStyle{
    Small, Normal, Big
};

enum class Key {
    // Буквы (физические клавиши)
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    // Цифры
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

    // Функциональные
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

    // Стрелки
    Up, Down, Left, Right,

    // Служебные
    Escape, Space, Shift, Ctrl, Alt, Tab, CapsLock, Enter, Backspace, Tilde,
    Insert, Delete, Home, End, PageUp, PageDown,

    Count  // для размера массивов
};

struct ConBackChar {
    wchar_t ch;
    uint4_t fgIdx;
    uint4_t bgIdx;
};

const std::array<ColorRGB, 16> CONBACK_DEFAULT_PALETTE = {{
    {0, 0, 0}, {128, 0, 0}, {0, 128, 0}, {128, 128, 0},
    {0, 0, 128}, {128, 0, 128}, {0, 128, 128}, {192, 129, 129},
    {128, 128, 128}, {255, 0, 0}, {0, 255, 0}, {255, 255, 0},
    {0, 0, 255}, {255, 0, 255}, {0, 255, 255}, {255, 255, 255}
}};

const uint4_t DEFAULT_FG_IDX = 15;
const uint4_t DEFAULT_BG_IDX = 0; 

struct Config{
    Alignment alignment = Alignment::LeftTop;
    std::array<ColorRGB, 16> pallete = CONBACK_DEFAULT_PALETTE;
    PixelStyle style = PixelStyle::Normal;
    bool cursorVisibility = true;
};

class ConsoleBackend
{
private:
    
    Alignment m_alignment = Alignment::LeftTop;
    
    std::array<ColorRGB, 16> m_pallete = CONBACK_DEFAULT_PALETTE;

    PixelStyle m_style = PixelStyle::Normal;

    uint4_t m_fgIdx = DEFAULT_FG_IDX;
    uint4_t m_bgIdx = DEFAULT_BG_IDX;

    #ifdef _WIN32

        std::unordered_map<int, bool> currentState; 
        std::unordered_map<int, bool> previousState;

        int ktc(Key k);

        HANDLE hConsoleOut;
        HANDLE hConsoleIn;
        CONSOLE_SCREEN_BUFFER_INFOEX originalConsoleInfo;

        static bool IsConsoleInFocus();

        WORD m_originalTextAttr = 0;

    #elif __linux__

        std::unordered_map<std::string, bool> currentState;
        std::unordered_map<std::string, bool> previousState;

        std::string ktc(Key k);

        struct termios orig_termios;

    #endif


public:
    ~ConsoleBackend();

    std::vector<std::string> split(const std::string& s, char delimiter); 

    void init(Config config);
    void init();

    void shutdown();

    void clearScreen();

    // print functions

    void printStr(const char* str);
    void printStr(const char* str, int cursorPosX, int cursorPosY);

    void printStr16Colors(const std::vector<ConBackChar>& str);
    void printStr16Colors(const std::vector<ConBackChar>& str, int cursorPosX, int cursorPosY);

    void printImgTrueColor(std::vector<ColorRGB> pixels, int width);

    void printImg16Colors(std::vector<uint4_t> indices, int width);

    // terminal actions

    void getTerminalSize(int& width, int& height);

    void setTerminalSize(int width, int height);

    // setters

    void setAlignment(Alignment alignment);

    void setPallete(std::array<ColorRGB, 16> pallete);

    void setFgColorIndex(uint4_t fgIdx);

    void setBgColorIndex(uint4_t bgIdx);

    void setPixelStyle(PixelStyle style);

    // input handle

    bool isKeyPressed(Key key);
    
    bool isKeyReleased(Key key);

    bool isKeyJustPressed(Key key);

    bool isKeyJustReleased(Key key);
    
    void updateInput();

    // cursor actions

    void setCursorPos(int x, int y);

    void setCursorVisibility(bool visibility);

    
};

} // namespace conback

