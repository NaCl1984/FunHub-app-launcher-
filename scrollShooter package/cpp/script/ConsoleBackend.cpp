#include "ConsoleBackend.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <string>
using namespace std;

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>
        #include <unistd.h>
#endif

#ifdef __linux__
    struct termios ConsoleBackend::old_termios;
    bool ConsoleBackend::input_initialized = false;
#endif

ConsoleBackend::ConsoleBackend() {
    // конструктор 
}

ConsoleBackend::~ConsoleBackend() {
    // деструктор – обычно пустой, если есть shutdown()
    shutdown();
}

bool ConsoleBackend::init(int screenWidth, int screenHeight){
    width = screenWidth;
    height = screenHeight;
    
    #ifdef __linux__
    if (input_initialized) return true;
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 0;
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
    input_initialized = true;
    #endif

    #ifdef _WIN32
        hStdin = GetStdHandle(STD_INPUT_HANDLE);
        if (hStdin != INVALID_HANDLE_VALUE) {
            GetConsoleMode(hStdin, &originalConsoleMode);
            // Убираем строковый ввод и эхо
            SetConsoleMode(hStdin, originalConsoleMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
        }
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        GetConsoleMode(hOut, &mode);
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, mode);
        CONSOLE_CURSOR_INFO ci;
        GetConsoleCursorInfo(hOut, &ci);
        ci.bVisible = FALSE;
        SetConsoleCursorInfo(hOut, &ci);
    #else
        // Для Unix: просто скрываем курсор ANSI-кодом (если терминал поддерживает)
        std::cout << "\033[?25l";
        // Отключаем синхронизацию с stdio для ускорения
        std::ios_base::sync_with_stdio(false);
        // Можно также отключить привязку cin к cout
        std::cin.tie(nullptr);
    #endif

    return true;
}


void ConsoleBackend::present(std::vector<std::vector<Pixel>>& screenBuffer,
                std::vector<std::vector<Pixel>>& prewFrame) {
    string output;
    output.reserve(width * height * 4); // Резервируем память заранее, чтобы избежать лишних аллокаций

    Rgb fg;
    Rgb bg;
    string ch;

    TerminalSize termSize = getTerminalSize();
    int spacer = (termSize.cols - width) / 2;

    

    for (int y = 0; y < height; y += 2) {
        
        output += "\033[0m";
        output.append(spacer, ' ');
        prewFgR = prewFgG = prewFgB = -1;
        prewBgR = prewBgG = prewBgB = -1;

        for (int x = 0; x < width; ++x) {
            const Pixel& upper = screenBuffer[y][x];
            const Pixel& lower = screenBuffer[y + 1][x];
            // const Pixel& prewUpper = prewFrame[y][x];
            // const Pixel& prewLower = prewFrame[y + 1][x];

            

            // if((upper.color.r == prewUpper.color.r && upper.color.g == prewUpper.color.g && upper.color.b == prewUpper.color.b) && (lower.color.r == prewLower.color.r && lower.color.g == prewLower.color.g && lower.color.b == prewLower.color.b) && upper.transparent == prewUpper.transparent && lower.transparent == prewLower.transparent){
            //     output += "\033[C";
            //     continue;
            // }

            if (!upper.transparent && !lower.transparent) {
                // оба есть – верхний полублок цветом текста, нижний – фоном
                ch = "▀";
                fg = upper.color;
                bg = lower.color;
            } else if (!upper.transparent && lower.transparent) {
                // только верхний – рисуем ▀, фон чёрный
                ch = "▀";
                fg = upper.color;
                bg = {0,0,0};
            } else if (upper.transparent && !lower.transparent) {
                // только нижний – рисуем ▄, фон чёрный
                ch = "▄";
                fg = lower.color;
                bg = {0,0,0};
            } else {
                // output += "\033[C";
                // continue;
                ch = " ";
                fg = {0,0,0};
                bg = {0,0,0};
            }

            string fontColor =  std::to_string(fg.r) + ";" + std::to_string(fg.g) + ";" + std::to_string(fg.b);
            string bgColor =  std::to_string(bg.r) + ";" + std::to_string(bg.g) + ";" + std::to_string(bg.b);

            if (fg.r != prewFgR || fg.g != prewFgG || fg.b != prewFgB) {
                output += "\033[38;2;" + std::to_string(fg.r) + ";" + std::to_string(fg.g) + ";" + std::to_string(fg.b) + "m";
                prewFgR = fg.r; prewFgG = fg.g; prewFgB = fg.b;
            }
            if (bg.r != prewBgR || bg.g != prewBgG || bg.b != prewBgB) {
                output += "\033[48;2;" + std::to_string(bg.r) + ";" + std::to_string(bg.g) + ";" + std::to_string(bg.b) + "m";
                prewBgR = bg.r; prewBgG = bg.g; prewBgB = bg.b;
            }

            output += ch;
        }
        output += "\n";
    }
    // prewFrame = screenBuffer;
    output += "\033[0m";
    #ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD pos = {0, 0};
        SetConsoleCursorPosition(hOut, pos);
        DWORD written;
        WriteConsoleA(hOut, output.c_str(), output.size(), &written, nullptr); // или WriteConsoleW если нужны широкие
    #else
        output = "\033[H" + output;    
        write(STDOUT_FILENO, output.c_str(), output.size());
    #endif
}

int ConsoleBackend::getKey() {
#ifdef _WIN32
    if (_kbhit()) {
        int ch = _getch();
        if (ch == 224 || ch == 0) { // расширенная клавиша
            ch = _getch();
            switch (ch) {
                case 72: return 1000; // вверх
                case 80: return 1001; // вниз
                case 75: return 1002; // влево
                case 77: return 1003; // вправо
                default: return 0;
            }
        }
        return ch;
    }
#else
    char ch;
    if (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == 27) { // ESC
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) == 1 && seq[0] == '[') {
                if (read(STDIN_FILENO, &seq[1], 1) == 1) {
                    switch (seq[1]) {
                        case 'A': return 1000;
                        case 'B': return 1001;
                        case 'C': return 1003;
                        case 'D': return 1002;
                    }
                }
            }
            return 27; // одиночный ESC
        }
        return ch;
    }
#endif
    return 0;
}

void ConsoleBackend::shutdown() {
#ifdef __linux__
    if (input_initialized) {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
        input_initialized = false;
    }
    std::cout << "\033[?25h";   // показать курсор
    std::cout.flush();
#endif

#ifdef _WIN32
    if (hStdin != INVALID_HANDLE_VALUE && originalConsoleMode) {
            SetConsoleMode(hStdin, originalConsoleMode);
            // Показать курсор (если скрывали)
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            CONSOLE_CURSOR_INFO ci;
            GetConsoleCursorInfo(hOut, &ci);
            ci.bVisible = TRUE;
            SetConsoleCursorInfo(hOut, &ci);
        }
#endif

}

TerminalSize ConsoleBackend::getTerminalSize(){
    
    TerminalSize size;

    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hConsole, &csbi)) {
            size.cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            size.rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        }
    #else
        struct winsize ws;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
            size.rows = ws.ws_row;
            size.cols = ws.ws_col;
        }
    #endif

        return size;
}