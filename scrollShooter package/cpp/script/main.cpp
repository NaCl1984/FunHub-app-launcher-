#include <iostream>
#include <vector>
#include <string>
#include "sprites.h"   // ваш сгенерированный файл
#include <fcntl.h>
#include <io.h>
#include <thread>
#include <chrono>
#include "levels.h"

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

#ifdef __linux__
    static struct termios old_termios;
    static bool input_initialized = false;
#endif

using namespace std;

const int SCREEN_W = 150;
const int SCREEN_H = 120;

std::vector<std::vector<Pixel>> screenBuffer(SCREEN_H, std::vector<Pixel>(SCREEN_W));
std::vector<std::vector<Pixel>> prewFrame(SCREEN_H, std::vector<Pixel>(SCREEN_W));

struct Coords{
    int x, y;

    Coords(int x, int y){
        this-> x = x;
        this-> y = y;
    }
};

struct Ship{
    int x;
    int y;
    int height = 1;
    int startX, startY;

    void move(char dir){
        if (dir == 'r'){
            if(x + 2 <= 48){
            this->x += 2;
            this->y += 1;}
        }
        else if (dir == 'l'){
            if(x - 2 >= 8){
            this->x -= 2;
            this->y -= 1;}
        }
        else if (dir == 'u' && (height + 1) <= 5){
            this->height += 1;
            this->y -= 5;
        }
        else if (dir == 'd' && (height - 1) >= 1){
            this->height -= 1;
            this->y += 5;
        }


        if(height > 5)
            height = 5;
        else if (height < 1)
            height = 1;
    }

};

void initScreen() {
    for (int y = 0; y < SCREEN_H; ++y)
        for (int x = 0; x < SCREEN_W; ++x){
            screenBuffer[y][x] = {{0,0,0}, true};
        }
}

void initInput() {
#ifdef __linux__
    if (input_initialized) return;
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
}

void restoreInput() {
#ifdef __linux__
    if (input_initialized) {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
        input_initialized = false;
    }
#endif
}

template<int Rows, int Cols>
void drawSprite(int x, int y, const Pixel (&sprite)[Rows][Cols]) {
    for (int dy = 0; dy < Rows; ++dy) {
        for (int dx = 0; dx < Cols; ++dx) {
            const Pixel& pix = sprite[dy][dx];
            if (!pix.transparent) {
                int bx = x + dx;
                int by = y + dy;
                if (bx >= 0 && bx < SCREEN_W && by >= 0 && by < SCREEN_H){
                    screenBuffer[by][bx] = pix;
                }
            }
        }
    }
}

void render() {
    string output;
    string prewFontColor;
    string prewBgColor;
    Rgb fg;
    Rgb bg;
    string ch;
    for (int y = 0; y < SCREEN_H; y += 2) {
        for (int x = 0; x < SCREEN_W; ++x) {
            const Pixel& upper = screenBuffer[y][x];
            const Pixel& lower = screenBuffer[y + 1][x];
            const Pixel& prewUpper = prewFrame[y][x];
            const Pixel& prewLower = prewFrame[y + 1][x];

            // if((upper.color.r == prewUpper.color.r && upper.color.g == prewUpper.color.g && upper.color.b == prewUpper.color.b) && (lower.color.r == prewLower.color.r && lower.color.g == prewLower.color.g && lower.color.b == prewLower.color.b)){
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

            if (fontColor != prewFontColor){
                output += "\033[38;2;" + fontColor + "m";
                prewFontColor = fontColor;
            }
            if (bgColor != prewBgColor){
                output += "\033[48;2;" + bgColor + "m";
                prewBgColor = bgColor;
            }

            output += ch;
        }
        output += "\033[0m\n";
    }
    prewFrame = screenBuffer;
    #ifdef _WIN32
        // Windows: прямой вывод через консольный API
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD pos = {0, 0};
        SetConsoleCursorPosition(hOut, pos);
        DWORD written;
        WriteConsoleA(hOut, output.c_str(), output.size(), &written, nullptr); // или WriteConsoleW если нужны широкие
        // Но у вас output — это std::string (UTF-8), поэтому WriteConsoleA подойдёт (если кодовая страница UTF-8)
    #else
        output = "\033[H" + output;    
    // Unix-подобные системы: атомарный вывод через write
        write(STDOUT_FILENO, output.c_str(), output.size());
        // Можно также использовать std::cout, но write гарантирует один системный вызов
    #endif
}

void setupConsole() {
#ifdef _WIN32
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
}

int getKeyPressed() {
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

Coords screenToLogic(int screenX, int screenY, int startX, int startY, int levelLenght, int pixelOffset){
    int xOffset = 8;
    int yOffset = 4;

    screenX = screenX - 5;
    screenY = screenY - 5;
    
    float A = (screenY - startY + (levelLenght * yOffset) - pixelOffset)/yOffset;
    float B = (screenX - startX - (levelLenght * xOffset) + 2 * pixelOffset)/xOffset;

    int logicX = (int)round((A + B) / 2);
    if (logicX > 5)
        logicX = 5;
    else if (logicX < 0)
        logicX = 0;

    int logicY = (int)round((A - B) / 2);
    if(logicY > levelLenght - 1)
        logicY = levelLenght - 1;
    else if (logicY < 0)
        logicY = 0;

    Coords logicCoords(logicX - 2, logicY - 2);

    return logicCoords;
}

template<int Rows, int Cols>
void gameLoop(const levelTile (&level)[Rows][Cols]){
    Ship player;
    player.x = 20;
    player.y = 80;

    Coords ancorPointShip(16, 8);
    
    int keyCode;
    int xOffset = 8;
    int yOffset = 4;
    int startX = 0;
    int startY = 80 ;

    int j;

    Coords logicShipCoords = screenToLogic(player.x + ancorPointShip.x, player.y + ancorPointShip.y, startX, startY, Rows, 0);
    int logicShipX = logicShipCoords.x;
    int logicShipY = logicShipCoords.y;

    levelTile renderOrder[Rows][Cols];
    for(int y = 0; y < Rows; ++y){
        for(int x = 0; x < Cols; ++x){
            renderOrder[y][x] = levelTile();
        }
    }

    renderOrder[logicShipY][logicShipX].isShip = true;  
    renderOrder[logicShipY + 1][logicShipX].isShip = true;
    renderOrder[logicShipY + 2][logicShipX].isShip = true;
   


    while (true){
        
        for (int pixelOffest = 0; pixelOffest < Rows * 4 ; ++pixelOffest){
            initScreen();
            for (int y = 0; y < Rows; ++y){
                for(int x = 0; x < Cols; ++x){
                    j = y - Rows;
                    
                    int tileY = startY + (j * yOffset) + (x * yOffset) + pixelOffest ;
                    int tileX = startX - (j * xOffset) + (x * xOffset) - pixelOffest * 2;

                    logicShipCoords = screenToLogic(player.x + ancorPointShip.x, player.y + ancorPointShip.y + (player.height * 5), startX, startY, Rows, pixelOffest);
                    
                    if(logicShipX != logicShipCoords.x){
                        renderOrder[logicShipY][logicShipX].isShip = false;
                        renderOrder[logicShipY + 1][logicShipX].isShip = false;
                        renderOrder[logicShipY + 2][logicShipX].isShip = false;

                        logicShipX = logicShipCoords.x;

                        renderOrder[logicShipY][logicShipX].isShip = true;
                        renderOrder[logicShipY + 1][logicShipX].isShip = true;
                        renderOrder[logicShipY + 2][logicShipX].isShip = true;
                    }
                    if(logicShipY != logicShipCoords.y){
                        renderOrder[logicShipY][logicShipX].isShip = false;
                        renderOrder[logicShipY + 1][logicShipX].isShip = false;
                        renderOrder[logicShipY + 2][logicShipX].isShip = false;

                        logicShipY = logicShipCoords.y;

                        renderOrder[logicShipY][logicShipX].isShip = true;
                        renderOrder[logicShipY + 1][logicShipX].isShip = true;
                        renderOrder[logicShipY + 2][logicShipX].isShip = true;
                    }

                    // cout << player.x << " " << player.y << endl;

                    if(level[y][x].type == 1)
                        drawSprite(tileX, tileY, tile);
                    
                    if (renderOrder[y][x].isShip){
                        drawSprite(player.x, player.y + (player.height * 5), shipShadow);
                        drawSprite(tileX, tileY, tileHighlight);
                        drawSprite(player.x, player.y, ship);
                    }

                    if(level[y][x].type == 2){
                        for(int height = 0; height < level[y][x].height; ++height){
                            drawSprite(tileX, tileY - (height * 8), wall);
                        }
                    }

                    

                    
                    // level[player.y][player.x].isShip = false;
                    // if ((player.y < Rows - 1) || (player.y > 0))
                    //     level[player.y - 1][player.x].isShip = true;
                    
                }
            }

            keyCode = getKeyPressed();

            if (keyCode == 27) {
                break;
            }
            // else if (keyCode == 32) {
            //     // стрельба – пока ничего не делаем, но можно вызвать функцию shoot()
            // }

            else if (keyCode == 1000){
                player.move('u');
            }
            else if (keyCode == 1001){
                player.move('d');
            }
            else if (keyCode == 1002){
                player.move('l');
            }
            else if (keyCode == 1003){
                player.move('r');
            }

            render();
            std::this_thread::sleep_for(std::chrono::microseconds(34)); 
        }
    }
    return;
}

void mainMenuLoop(){

}


uint8_t state = 2;

int main() {
    setupConsole();
    initScreen();
    initInput();
    std::ios_base::sync_with_stdio(false);

    while (true){
        if (state == 1){
            mainMenuLoop();
        }
        else if (state == 0){
            break;
        }
        else if(state == 2){
            gameLoop(level1);
            state = 0;
        }
    }
   

    restoreInput();
    return 0;
}