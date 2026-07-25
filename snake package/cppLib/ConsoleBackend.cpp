#ifndef NOMINMAX
    #define NOMINMAX
#endif

#ifdef min
    #undef min
#endif

#ifdef max
    #undef max
#endif

#include "ConsoleBackend.h"
#include <array>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include "uint4_t.h"
#include <algorithm>

// includes



#ifdef _WIN32

    #include <winuser.h>
    #include <windows.h>
    #pragma comment(lib, "user32.lib")

#elif __linux__

    #include <poll.h>
    #include <unistd.h>
    #include <termios.h> 
    #include <fcntl.h>
    #include <codecvt>
    #include <locale>

#endif



namespace conback{

std::vector<std::string> ConsoleBackend::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = s.find(delimiter);
    while (end != std::string::npos) {
        tokens.push_back(s.substr(start, end - start));
        start = end + 1;
        end = s.find(delimiter, start);
    }
    tokens.push_back(s.substr(start));
    return tokens;
}

#ifdef _WIN32

bool ConsoleBackend::IsConsoleInFocus() {
    HWND consoleHwnd = GetConsoleWindow();
    if (!consoleHwnd) return false;

    HWND foregroundHwnd = GetForegroundWindow();
    if (!foregroundHwnd) return false;

    // 1. Простая проверка для классической консоли (conhost.exe)
    if (foregroundHwnd == consoleHwnd) return true;

    // 2. Проверка для Windows Terminal (проверяем всю иерархию окон)
    HWND parentHwnd = consoleHwnd;
    while (parentHwnd != NULL) {
        if (parentHwnd == foregroundHwnd) return true;
        parentHwnd = GetParent(parentHwnd);
    }

    // 3. Дополнительный фоллбек: проверяем, принадлежит ли фокусное окно
    // тому же потоку/дереву UI
    DWORD consoleProcId = 0;
    DWORD foregroundProcId = 0;
    GetWindowThreadProcessId(consoleHwnd, &consoleProcId);
    GetWindowThreadProcessId(foregroundHwnd, &foregroundProcId);

    return (consoleProcId != 0 && consoleProcId == foregroundProcId);
}

#endif

// Key to OS key code


#ifdef _WIN32

int ConsoleBackend::ktc(Key k){
    
    switch (k) {
        // Буквы
        case Key::A: return 'A';
        case Key::B: return 'B';
        case Key::C: return 'C';
        case Key::D: return 'D';
        case Key::E: return 'E';
        case Key::F: return 'F';
        case Key::G: return 'G';
        case Key::H: return 'H';
        case Key::I: return 'I';
        case Key::J: return 'J';
        case Key::K: return 'K';
        case Key::L: return 'L';
        case Key::M: return 'M';
        case Key::N: return 'N';
        case Key::O: return 'O';
        case Key::P: return 'P';
        case Key::Q: return 'Q';
        case Key::R: return 'R';
        case Key::S: return 'S';
        case Key::T: return 'T';
        case Key::U: return 'U';
        case Key::V: return 'V';
        case Key::W: return 'W';
        case Key::X: return 'X';
        case Key::Y: return 'Y';
        case Key::Z: return 'Z';

        // Цифры
        case Key::Num0: return '0';
        case Key::Num1: return '1';
        case Key::Num2: return '2';
        case Key::Num3: return '3';
        case Key::Num4: return '4';
        case Key::Num5: return '5';
        case Key::Num6: return '6';
        case Key::Num7: return '7';
        case Key::Num8: return '8';
        case Key::Num9: return '9';

        // Функциональные
        case Key::F1:  return VK_F1;
        case Key::F2:  return VK_F2;
        case Key::F3:  return VK_F3;
        case Key::F4:  return VK_F4;
        case Key::F5:  return VK_F5;
        case Key::F6:  return VK_F6;
        case Key::F7:  return VK_F7;
        case Key::F8:  return VK_F8;
        case Key::F9:  return VK_F9;
        case Key::F10: return VK_F10;
        case Key::F11: return VK_F11;
        case Key::F12: return VK_F12;

        // Стрелки
        case Key::Up:    return VK_UP;
        case Key::Down:  return VK_DOWN;
        case Key::Left:  return VK_LEFT;
        case Key::Right: return VK_RIGHT;

        // Служебные
        case Key::Escape:    return VK_ESCAPE;
        case Key::Space:     return VK_SPACE;
        case Key::Shift:     return VK_SHIFT;
        case Key::Ctrl:      return VK_CONTROL;
        case Key::Alt:       return VK_MENU;
        case Key::Tab:       return VK_TAB;
        case Key::CapsLock:  return VK_CAPITAL;
        case Key::Enter:     return VK_RETURN;
        case Key::Backspace: return VK_BACK;
        case Key::Tilde:     return VK_OEM_3; 
        case Key::Insert:    return VK_INSERT;
        case Key::Delete:    return VK_DELETE;
        case Key::Home:      return VK_HOME;
        case Key::End:       return VK_END;
        case Key::PageUp:    return VK_PRIOR;
        case Key::PageDown:  return VK_NEXT;

        default: return 0;
    }
}

#elif __linux__

std::string ConsoleBackend::ktc(Key k){
    switch (k) {
    // Буквы (возвращаем заглавные, чтобы соответствовать физической клавише)
    case Key::A: return "A";
    case Key::B: return "B";
    case Key::C: return "C";
    case Key::D: return "D";
    case Key::E: return "E";
    case Key::F: return "F";
    case Key::G: return "G";
    case Key::H: return "H";
    case Key::I: return "I";
    case Key::J: return "J";
    case Key::K: return "K";
    case Key::L: return "L";
    case Key::M: return "M";
    case Key::N: return "N";
    case Key::O: return "O";
    case Key::P: return "P";
    case Key::Q: return "Q";
    case Key::R: return "R";
    case Key::S: return "S";
    case Key::T: return "T";
    case Key::U: return "U";
    case Key::V: return "V";
    case Key::W: return "W";
    case Key::X: return "X";
    case Key::Y: return "Y";
    case Key::Z: return "Z";

    // Цифры
    case Key::Num0: return "0";
    case Key::Num1: return "1";
    case Key::Num2: return "2";
    case Key::Num3: return "3";
    case Key::Num4: return "4";
    case Key::Num5: return "5";
    case Key::Num6: return "6";
    case Key::Num7: return "7";
    case Key::Num8: return "8";
    case Key::Num9: return "9";

    // Функциональные клавиши F1-F12 (стандартные escape-последовательности xterm)
    case Key::F1:  return "\x1bOP";
    case Key::F2:  return "\x1bOQ";
    case Key::F3:  return "\x1bOR";
    case Key::F4:  return "\x1bOS";
    case Key::F5:  return "\x1b[15~";
    case Key::F6:  return "\x1b[17~";
    case Key::F7:  return "\x1b[18~";
    case Key::F8:  return "\x1b[19~";
    case Key::F9:  return "\x1b[20~";
    case Key::F10: return "\x1b[21~";
    case Key::F11: return "\x1b[23~";
    case Key::F12: return "\x1b[24~";

    // Стрелки
    case Key::Up:    return "\x1b[A";
    case Key::Down:  return "\x1b[B";
    case Key::Left:  return "\x1b[D";
    case Key::Right: return "\x1b[C";

    // Служебные клавиши
    case Key::Escape:    return "\x1b";       // ESC (27)
    case Key::Space:     return " ";          // пробел
    case Key::Shift:     return "Shift";      // модификаторы не имеют стандартной последовательности
    case Key::Ctrl:      return "Ctrl";
    case Key::Alt:       return "Alt";
    case Key::Tab:       return "\t";         // табуляция
    case Key::CapsLock:  return "CapsLock";   // обычно не отслеживается
    case Key::Enter:     return "\n";         // в raw-режиме Enter даёт \n (10)
    case Key::Backspace: return "\x7f";       // DEL (127)
    case Key::Tilde:     return "`";          // клавиша с тильдой (без Shift)
    case Key::Insert:    return "\x1b[2~";
    case Key::Delete:    return "\x1b[3~";
    case Key::Home:      return "\x1b[H";     // или \x1b[1~ (но \x1b[H чаще)
    case Key::End:       return "\x1b[F";     // или \x1b[4~
    case Key::PageUp:    return "\x1b[5~";
    case Key::PageDown:  return "\x1b[6~";

    default: return "";
    }
}

#endif

void ConsoleBackend::init(Config config){
    m_alignment = config.alignment;
    m_pallete = config.pallete;
    m_style = config.style;

    init();

    setCursorVisibility(config.cursorVisibility);
}

void ConsoleBackend::init(){

    #ifdef _WIN32
        
        hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
        hConsoleIn  = GetStdHandle(STD_INPUT_HANDLE);
        if (hConsoleOut == INVALID_HANDLE_VALUE || hConsoleIn == INVALID_HANDLE_VALUE) {
            return;
        }

        originalConsoleInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
        if (!GetConsoleScreenBufferInfoEx(hConsoleOut, &originalConsoleInfo)) {
            return;
        }

        DWORD outMode;
        GetConsoleMode(hConsoleOut, &outMode);
        outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hConsoleOut, outMode);

        std::array<ColorRGB, 16> &pl = this->m_pallete;

        setPallete(pl);

        CONSOLE_SCREEN_BUFFER_INFOEX info = originalConsoleInfo;
        
        info.wAttributes = (DEFAULT_BG_IDX << 4) | DEFAULT_FG_IDX;

        SetConsoleScreenBufferInfoEx(hConsoleOut, &info);
        

        WriteConsoleA(hConsoleOut, "\x1b[2J\x1b[H", 8, NULL, NULL);
        
    #elif __linux__
        tcgetattr(STDIN_FILENO, &orig_termios);

        struct termios raw = orig_termios;
        cfmakeraw(&raw);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);

        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    #endif
}

void ConsoleBackend::shutdown(){

    #ifdef _WIN32
        
        WriteConsoleA(hConsoleOut, "\x1b[?25h", 6, NULL, NULL);
        SetConsoleScreenBufferInfoEx(hConsoleOut, &originalConsoleInfo);
        if (m_originalTextAttr != 0) {
            SetConsoleTextAttribute(hConsoleOut, m_originalTextAttr);
        }

    #elif __linux__

        write(STDOUT_FILENO, "\x1b[?25h", 6);

        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);

        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);

    #endif
}

ConsoleBackend::~ConsoleBackend() {
    shutdown();
}

void ConsoleBackend::clearScreen(){
    #ifdef _WIN32

        if (hConsoleOut == INVALID_HANDLE_VALUE) return;

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (!GetConsoleScreenBufferInfo(hConsoleOut, &csbi)) return;

        DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
        DWORD count;
        COORD homeCoords = { 0, 0 };

        FillConsoleOutputCharacterA(hConsoleOut, ' ', cellCount, homeCoords, &count);

        FillConsoleOutputAttribute(hConsoleOut, csbi.wAttributes, cellCount, homeCoords, &count);

        SetConsoleCursorPosition(hConsoleOut, homeCoords);

    #elif __linux__
        printStr("\033[2J\033[H");
    #endif  
}

// print functions

void ConsoleBackend::printStr(const char* str){

    #ifdef _WIN32

        int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);

        std::wstring wideStr(len, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, str, -1, &wideStr[0], len);

        WriteConsoleW(hConsoleOut, wideStr.c_str(), static_cast<DWORD>(wideStr.size() - 1), nullptr, nullptr);

    #elif __linux__

        // ColorRGB &fg = m_pallete[m_fgIdx];
        // ColorRGB &bg = m_pallete[m_bgIdx];

        // str = 
        //     "\033[38;2;" + std::to_string(fg.r) + ";" + std::to_string(fg.g) + ";" + std::to_string(fg.b) + 
        //     "m\033[48;2;" + std::to_string(bg.r) + ";" + std::to_string(bg.g) + ";" + std::to_string(bg.b) + "m" + 
        //     str + 
        //     "\033[0m";

        write(STDOUT_FILENO, str, strlen(str));

    #endif

}

void ConsoleBackend::printStr(const char* str, int cursorPosX, int cursorPosY){

    setCursorPos(cursorPosX, cursorPosY);

    printStr(str);

}

void ConsoleBackend::printStr16Colors(const std::vector<ConBackChar>& str){
    if (str.empty()) return;

    #ifdef _WIN32

        std::vector<CHAR_INFO> buffer(str.size());

        for (size_t i = 0; i < str.size(); ++i) {
            buffer[i].Char.UnicodeChar = str[i].ch;
            buffer[i].Attributes = (str[i].bgIdx << 4) | str[i].fgIdx;
        }

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (!GetConsoleScreenBufferInfo(hConsoleOut, &csbi)) return;

        COORD bufferSize = { static_cast<SHORT>(str.size()), 1 };
        COORD bufferCoord = { 0, 0 };
        SMALL_RECT writeRegion;
        writeRegion.Left   = csbi.dwCursorPosition.X;
        writeRegion.Top    = csbi.dwCursorPosition.Y;
        writeRegion.Right  = csbi.dwCursorPosition.X + static_cast<SHORT>(str.size()) - 1;
        writeRegion.Bottom = csbi.dwCursorPosition.Y;

        WriteConsoleOutputW(
            hConsoleOut,
            buffer.data(),
            bufferSize,
            bufferCoord,
            &writeRegion
        );

        COORD newCursorPos = { static_cast<SHORT>(csbi.dwCursorPosition.X + str.size()), csbi.dwCursorPosition.Y };
        SetConsoleCursorPosition(hConsoleOut, newCursorPos);

    #elif __linux__

        const ColorRGB& fg = m_pallete[m_fgIdx];
        const ColorRGB& bg = m_pallete[m_bgIdx];

        std::string restoreColors = 
            "\033[38;2;" + std::to_string(fg.r) + ";" + std::to_string(fg.g) + ";" + std::to_string(fg.b) + "m" +
            "\033[48;2;" + std::to_string(bg.r) + ";" + std::to_string(bg.g) + ";" + std::to_string(bg.b) + "m";

        std::string outStr;
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

        for (size_t i = 0; i < str.size(); ++i) {
            ColorRGB &curFg = m_pallete[str[i].fgIdx];
            ColorRGB &curBg = m_pallete[str[i].bgIdx];
            
            outStr +=  
                "\033[38;2;" + std::to_string(curFg.r) + ";" + std::to_string(curFg.g) + ";" + std::to_string(curFg.b) + "m" +
                "\033[48;2;" + std::to_string(curBg.r) + ";" + std::to_string(curBg.g) + ";" + std::to_string(curBg.b) + "m";

            if (str[i].ch < 0x80) {
                outStr += static_cast<char>(str[i].ch);
            } else {
                outStr += converter.to_bytes(str[i].ch);
            }
        }

        outStr += restoreColors;

        printStr(outStr.c_str());

    #endif
}

void ConsoleBackend::printStr16Colors(const std::vector<ConBackChar>& str, int cursorPosX, int cursorPosY){
    setCursorPos(cursorPosX, cursorPosY);

    printStr16Colors(str);
}

void ConsoleBackend::printImgTrueColor(std::vector<ColorRGB> pixels, int width){

    //     ▀    \033[48;2;R;G;BmЦвет фона\033[0m   \033[38;2;R;G;BmЦвет текста\033[0m

    const ColorRGB& fg = m_pallete[m_fgIdx];
    const ColorRGB& bg = m_pallete[m_bgIdx];

    std::string restoreColors = 
        "\033[38;2;" + std::to_string(fg.r) + ";" + std::to_string(fg.g) + ";" + std::to_string(fg.b) + "m" +
        "\033[48;2;" + std::to_string(bg.r) + ";" + std::to_string(bg.g) + ";" + std::to_string(bg.b) + "m\n";

    std::string str;

    int rowsCount = (pixels.size() + width - 1) / width;

    int tWidth = 0, tHeight = 0;

    int xOffset = 0, yOffset = 0;

    getTerminalSize(tWidth, tHeight);

    int pixelWidthInCols = (m_style == PixelStyle::Big) ? 2 : 1;
    int maxWidth = tWidth / pixelWidthInCols;
    int maxHeight = tHeight * ((m_style == PixelStyle::Small) ? 2 : 1);

    switch(m_style){
        case PixelStyle::Small : 

            for(int y = 0; y < std::min(rowsCount, maxHeight); y += 2){
                for(int x = 0; x < std::min(width, maxWidth); ++x){
                    if(x + (width * y) + width < static_cast<int>(pixels.size())){
                        const ColorRGB &upP = pixels[x + (width * y)];
                        const ColorRGB &btmP = pixels[x + (width * y) + width];
                        
                        str += 
                            "\033[38;2;" + std::to_string(upP.r) + ";" + std::to_string(upP.g) + ";" + std::to_string(upP.b) + 
                            "m\033[48;2;" + std::to_string(btmP.r) + ";" + std::to_string(btmP.g) + ";" + std::to_string(btmP.b) + "m▀";
                    } else if (x + (width * y) < static_cast<int>(pixels.size())) {
                        const ColorRGB &upP = pixels[x + (width * y)];
                        
                        str += 
                            "\033[38;2;" + std::to_string(upP.r) + ";" + std::to_string(upP.g) + ";" + std::to_string(upP.b) + 
                            "m\033[48;2;0;0;0m▀";
                    } else {
                        str += "\033[48;2;0;0;0m ";
                    }
                }

                str += restoreColors;
            }
                        
            break;

        case PixelStyle::Normal :

            for(int y = 0; y < std::min(rowsCount, maxHeight); ++y){
                for(int x = 0; x < std::min(width, maxWidth) ; ++x){
                    if(x + (width * y) < static_cast<int>(pixels.size())){
                        const ColorRGB &p = pixels[x + (width * y)];
                        
                        str += "\033[48;2;" + std::to_string(p.r) + ";" + std::to_string(p.g) + ";" + std::to_string(p.b) + "m ";
                    } else {
                        str += "\033[48;2;0;0;0m ";
                    }
                }
               
                str += restoreColors;
            }

            break;

        case PixelStyle::Big :

            for(int y = 0; y < std::min(rowsCount, maxHeight); ++y){
                for(int x = 0; x < std::min(width, maxWidth) ; ++x){
                    if(x + (width * y) < static_cast<int>(pixels.size())){
                        const ColorRGB &p = pixels[x + (width * y)];
                        
                        str += "\033[48;2;" + std::to_string(p.r) + ";" + std::to_string(p.g) + ";" + std::to_string(p.b) + "m  ";
                    } else {
                        str += "\033[48;2;0;0;0m  ";
                    }
                }
               
                str += restoreColors;
            }

            break;

        default: break;
    }

    // horizontal shift

    if(m_alignment == Alignment::Center || m_alignment == Alignment::CenterBottom || m_alignment == Alignment::CenterTop){
        xOffset = std::max(0, (int)(tWidth / 2) - (int)((width * pixelWidthInCols) / 2));
    } 
    
    else if(m_alignment == Alignment::RightTop || m_alignment == Alignment::RightCenter || m_alignment == Alignment::RightBottom){
        xOffset = std::max(0, tWidth - (width * pixelWidthInCols));
    }

    // vertical shift

    int lCount = std::min(rowsCount, maxHeight);

    if(m_alignment == Alignment::LeftCenter || m_alignment == Alignment::Center || m_alignment == Alignment::RightCenter){
        yOffset = std::max(0, (int)(tHeight / 2) - (int)(lCount / 2) );
    }

    else if(m_alignment == Alignment::LeftBottom || m_alignment == Alignment::CenterBottom || m_alignment == Alignment::RightBottom){
        yOffset = std::max(0, tHeight - lCount);
    }

    std::string newStr;

    auto lines = split(str, '\n');

    for(std::string &line : lines){
        newStr += std::string(xOffset, ' ') + line + "\n";
    }

    newStr = std::string(yOffset, '\n') + newStr;

    printStr(newStr.c_str());
}

void ConsoleBackend::printImg16Colors(std::vector<uint4_t> indices, int width){

    #ifdef _WIN32

        if (indices.empty() || width <= 0) return;

        int rowsCount = (indices.size() + width - 1) / width;

        int tWidth = 0, tHeight = 0;
        getTerminalSize(tWidth, tHeight);

        int pixelWidthInCols = (m_style == PixelStyle::Big) ? 2 : 1;
        int maxWidth = tWidth / pixelWidthInCols;
        int maxHeight = tHeight * ((m_style == PixelStyle::Small) ? 2 : 1);

        COORD bufferSize;
        bufferSize.X = std::min(width, maxWidth) * ((m_style == PixelStyle::Big) ? 2 : 1);
        int rows = std::min(rowsCount, maxHeight);
        bufferSize.Y = (m_style == PixelStyle::Small) ? ((rows % 2 == 0) ? rows / 2 : (rows  + 1) / 2) : rows ;

        std::vector<CHAR_INFO> buffer(bufferSize.X * bufferSize.Y);

        COORD bufferCoord = { 0, 0 };
        SMALL_RECT writeRegion;

        int xOffset = 0, yOffset = 0;

        // horizontal shift

        if(m_alignment == Alignment::Center || m_alignment == Alignment::CenterBottom || m_alignment == Alignment::CenterTop){
            xOffset = std::max(0, (int)(tWidth / 2) - (int)((width * pixelWidthInCols) / 2));
        } 
        
        else if(m_alignment == Alignment::RightTop || m_alignment == Alignment::RightCenter || m_alignment == Alignment::RightBottom){
            xOffset = std::max(0, tWidth - (width * pixelWidthInCols));
        }

        // vertical shift

        int lCount = std::min(rowsCount, maxHeight);

        if(m_alignment == Alignment::LeftCenter || m_alignment == Alignment::Center || m_alignment == Alignment::RightCenter){
            yOffset = std::max(0, (int)(tHeight / 2) - (int)(lCount / 2) );
        }

        else if(m_alignment == Alignment::LeftBottom || m_alignment == Alignment::CenterBottom || m_alignment == Alignment::RightBottom){
            yOffset = std::max(0, tHeight - lCount);
        }

        writeRegion.Left   = (SHORT)xOffset;
        writeRegion.Top    = (SHORT)yOffset;
        writeRegion.Right  = (SHORT)(xOffset + bufferSize.X - 1);
        writeRegion.Bottom = (SHORT)(yOffset + bufferSize.Y - 1);


        switch(m_style){
            case PixelStyle::Small : 

                for(int y = 0; y < std::min(rowsCount, maxHeight); y += 2){
                    for(int x = 0; x < std::min(width, maxWidth); ++x){
                        
                        CHAR_INFO &ci = buffer[x + (bufferSize.X * y / 2)]; 

                        if(x + (width * y) + width < static_cast<int>(indices.size())){
                            const uint4_t &upP = indices[x + (width * y)];
                            const uint4_t &btmP = indices[x + (width * y) + width];
                            
                            ci.Char.UnicodeChar = L'\u2580';

                            ci.Attributes = (btmP << 4) | upP;

                        } 
                        
                        else if (x + (width * y) < static_cast<int>(indices.size())) {
                            const uint4_t &upP = indices[x + (width * y)];
                            
                            ci.Char.UnicodeChar = L'\u2580';

                            ci.Attributes = (0 << 4) | upP;
                        } 
                        
                        else {
                            ci.Char.UnicodeChar = L' ';

                            ci.Attributes = (0 << 4) | 0;
                        }
                    }

                }
                            
                break;

            case PixelStyle::Normal :

                for(int y = 0; y < std::min(rowsCount, maxHeight); ++y){
                    for(int x = 0; x < std::min(width, maxWidth) ; ++x){
                        
                        CHAR_INFO &ci = buffer[x + (bufferSize.X * y)]; 

                        if(x + (width * y) < static_cast<int>(indices.size())){
                            const uint4_t &p = indices[x + (width * y)];
                            
                            ci.Char.UnicodeChar = L' ';

                            ci.Attributes = (p << 4) | 0;
                            
                        } 
                        
                        else {
                            ci.Char.UnicodeChar = L' ';

                            ci.Attributes = (0 << 4) | 0;
                        }
                    }
                

                }

                break;

            case PixelStyle::Big :

                for(int y = 0; y < std::min(rowsCount, maxHeight); ++y){
                    for(int x = 0; x < std::min(width, maxWidth) ; ++x){
                        
                        CHAR_INFO &ci1 = buffer[x * 2 + (bufferSize.X * y)]; 
                        CHAR_INFO &ci2 = buffer[x * 2 + (bufferSize.X * y) + 1]; 

                        if(x + (width * y) < static_cast<int>(indices.size())){
                            const uint4_t &p = indices[x + (width * y)];
                            
                            ci1.Char.UnicodeChar = L' ';

                            ci1.Attributes = (p << 4) | 0;
                            ci2 = ci1;
                        } 
                        
                        else {
                            ci1.Char.UnicodeChar = L' ';

                            ci1.Attributes = (0 << 4) | 0;
                            ci2 = ci1;
                        }
                    }
                
                   
                }

                break;

            default: break;
        }

        WriteConsoleOutputW(
            hConsoleOut,
            buffer.data(),
            bufferSize,
            bufferCoord,
            &writeRegion
        );

    #elif __linux__

        std::vector<ColorRGB> pixels;

        for(int ind : indices){
            pixels.push_back(m_pallete[ind]);
        }

        printImgTrueColor(pixels, width);

    #endif

}

// terminal stuff

void ConsoleBackend::getTerminalSize(int& width, int& height){

    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hConsoleOut, &csbi)) {
            width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
            return;
        }
    #elif __linux__
        struct winsize ws;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
            width = ws.ws_col; 
            height = ws.ws_row;
            return;
        }
    #endif

    width = 80; 
    height = 24; // fallback

}

void ConsoleBackend::setTerminalSize(int width, int height){
    printStr("\033");
}

// setters

void ConsoleBackend::setAlignment(Alignment alignment){ m_alignment = alignment; }

void ConsoleBackend::setPallete(std::array<ColorRGB, 16> pallete){ 
    m_pallete = pallete; 

    #ifdef _WIN32

        std::ostringstream oss;
    for (int i = 0; i < 16; ++i) {
        oss << "\033]4;" << i << ";#" 
            << std::hex << std::setfill('0') << std::setw(2) << (int)m_pallete[i].b
            << std::hex << std::setfill('0') << std::setw(2) << (int)m_pallete[i].g
            << std::hex << std::setfill('0') << std::setw(2) << (int)m_pallete[i].r
            << "\033\\";
    }


    printStr(oss.str().c_str());

        CONSOLE_SCREEN_BUFFER_INFOEX info = originalConsoleInfo;
        for (int i = 0; i < 16; ++i) {
            info.ColorTable[i] = RGB(m_pallete[i].r, m_pallete[i].g, m_pallete[i].b);
        }
        // info.srWindow.Right += 1;
        // info.srWindow.Bottom += 1;
        SetConsoleScreenBufferInfoEx(hConsoleOut, &info);

    #endif
    
}

void ConsoleBackend::setFgColorIndex(uint4_t fgIdx){
    m_fgIdx = fgIdx;
    
    #ifdef _WIN32

    CONSOLE_SCREEN_BUFFER_INFOEX info;
    
    info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    if (!GetConsoleScreenBufferInfoEx(hConsoleOut, &info)) {
        return;
    }

    info.srWindow.Right += 1;
    info.srWindow.Bottom += 1;
    
    info.wAttributes = (m_bgIdx << 4) | m_fgIdx;

    SetConsoleScreenBufferInfoEx(hConsoleOut, &info);

    #endif
}

void ConsoleBackend::setBgColorIndex(uint4_t bgIdx){
    m_bgIdx = bgIdx;
    
    #ifdef _WIN32

    CONSOLE_SCREEN_BUFFER_INFOEX info;
    
    info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    if (!GetConsoleScreenBufferInfoEx(hConsoleOut, &info)) {
        return;
    }

    info.srWindow.Right += 1;
    info.srWindow.Bottom += 1;

    info.wAttributes = (m_bgIdx << 4) | m_fgIdx;

    SetConsoleScreenBufferInfoEx(hConsoleOut, &info);

    #endif
}

void ConsoleBackend::setPixelStyle(PixelStyle style){ m_style = style; }

// input handle

bool ConsoleBackend::isKeyPressed(Key key){ 
    #ifdef _WIN32

        if(!IsConsoleInFocus()) return false;

        int code = ktc(key);
        bool state = (GetAsyncKeyState(code) & 0x8000) != 0;
        currentState[code] = state;
        return state;

    #elif __linux__

        std::string code = ktc(key);
        bool state = currentState[code];
        return state;

    #endif
}

bool ConsoleBackend::isKeyReleased(Key key){
    #ifdef _WIN32

        if(!IsConsoleInFocus()) return false;

        int code = ktc(key);
        bool state = (GetAsyncKeyState(code) & 0x8000) != 0;
        currentState[code] = state;
        return !state;

    #elif __linux__

        std::string code = ktc(key);
        bool state = currentState[code];
        return !state;

    #endif
}

bool ConsoleBackend::isKeyJustPressed(Key key){
    #ifdef _WIN32

        if(!IsConsoleInFocus()) return false;

        int code = ktc(key);
        bool currState = (GetAsyncKeyState(code) & 0x8000) != 0;
        currentState[code] = currState;
        bool prevState = previousState[code];
        return currState && !prevState;
        

    #elif __linux__

        std::string code = ktc(key);
        bool curr = currentState[code];
        bool prev = previousState[code];
        return curr && !prev;

    #endif
}

bool ConsoleBackend::isKeyJustReleased(Key key){
    #ifdef _WIN32

        if(!IsConsoleInFocus()) return false;

        int code = ktc(key);
        bool currState = (GetAsyncKeyState(code) & 0x8000) != 0;
        currentState[code] = currState;
        bool prevState = previousState[code];
        return !currState && prevState;

    #elif __linux__

        std::string code = ktc(key);
        bool curr = currentState[code];
        bool prev = previousState[code];
        return !curr && prev;

    #endif
}

void ConsoleBackend::updateInput(){

    #ifdef _WIN32

        previousState = currentState;

    #elif __linux__

        struct pollfd pfd = {STDIN_FILENO, POLLIN, 0};
        if (poll(&pfd, 1, 0) <= 0) {
            return;
        }

        char buf[32];
        ssize_t n = read(STDIN_FILENO, buf, sizeof(buf) - 1);
        if (n <= 0) return;

        currentState.clear();

        size_t i = 0;
        while (i < n) {
            unsigned char c = buf[i];
            if (c == 27) { 
                std::string seq;
                seq.push_back(c);
                ++i;
                while (i < n && !(buf[i] >= 'A' && buf[i] <= 'Z') && buf[i] != '~') {
                    seq.push_back(buf[i]);
                    ++i;
                }
                if (i < n) {
                    seq.push_back(buf[i]);
                    ++i;
                }
                currentState[seq] = true;
            } else {
                std::string key(1, c);
                currentState[key] = true;
                ++i;
            }
        }

        previousState = currentState;

    #endif

}

// cursor actions

void ConsoleBackend::setCursorPos(int x, int y){

    #ifdef _WIN32

        COORD position = { static_cast<SHORT>(x), static_cast<SHORT>(y) };

        SetConsoleCursorPosition(hConsoleOut, position);

    #elif __linux__

        std::string pos = "\033[" + std::to_string(y + 1) + ";" + std::to_string(x + 1) + "H";
        
        write(STDOUT_FILENO, pos.c_str(), pos.size());

    #endif

}

void ConsoleBackend::setCursorVisibility(bool visibility){

    const char* seq = visibility ? "\x1b[?25h" : "\x1b[?25l";

    #ifdef _WIN32
        WriteConsoleA(hConsoleOut, seq, strlen(seq), NULL, NULL);
    #elif __linux__
        write(STDOUT_FILENO, seq, strlen(seq));
    #endif

}

} // namespace conback


