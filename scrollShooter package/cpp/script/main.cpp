#include <iostream>
#include <vector>
#include <string>
#include <windows.h>   // Только для Windows
#include "sprites.h"   // ваш сгенерированный файл
#include <fcntl.h>
#include <io.h>
#include "linux_conio.h"
#include <thread>
#include <chrono>
using namespace std;

const int SCREEN_W = 150;
const int SCREEN_H = 60;
const int WIRTUAL_H = SCREEN_H * 2;

// Глобальный буфер: каждый элемент – Glyph
std::vector<std::vector<Glyph>> bgLayer(SCREEN_H, std::vector<Glyph>(SCREEN_W));
std::vector<std::vector<Glyph>> fgLayer(SCREEN_H, std::vector<Glyph>(SCREEN_W));

struct levelTile{
    int8_t type;
    int8_t height;
    bool isShip;

    levelTile(int8_t type = 0, bool isShip = false, int8_t height = 1){
        this->type = type;
        this->height = height;
        this->isShip = isShip;
    }
};

// Инициализация буфера (прозрачные пробелы)
void initScreen() {
    for (int y = 0; y < SCREEN_H; ++y)
        for (int x = 0; x < SCREEN_W; ++x){
            bgLayer[y][x] = {L' ', {0,0,0}, {0,0,0}, true};
            fgLayer[y][x] = {L' ', {0,0,0}, {0,0,0}, true};
        }
}

template<int Rows, int Cols>
void drawSprite(int x, int y, const Glyph (&sprite)[Rows][Cols]) {
    for (int dy = 0; dy < Rows; ++dy) {
        for (int dx = 0; dx < Cols; ++dx) {
            const Glyph& g = sprite[dy][dx];
            if (!g.transparent) {
                int bx = x + dx;
                int by = y + dy;
                if (bx >= 0 && bx < SCREEN_W && by >= 0 && by < SCREEN_H){
                    if((g.bg.r != 0 || g.bg.g != 0 || g.bg.b != 0 )){
                        bgLayer[by][bx].bg = g.bg;
                        bgLayer[by][bx].transparent = false;
                    }
                        
                    if((g.fg.r != 0 || g.fg.g != 0 || g.fg.b != 0 )){

                        if(((fgLayer[by][bx].ch == L'▄' && g.ch == L'▀') || (fgLayer[by][bx].ch == L'▀' && g.ch == L'▄')) && (g.bg.r == 0 && g.bg.g == 0 && g.bg.b == 0 ) ){
                            bgLayer[by][bx].bg = fgLayer[by][bx].fg;
                            fgLayer[by][bx] = g;
                            fgLayer[by][bx].transparent = false;
                        }

                                
                        else{
                        fgLayer[by][bx] = g;
                        fgLayer[by][bx].transparent = false;
                        }
                    }
                }
            }
        }
    }
}

// Вывод буфера на экран с помощью ANSI
void render() {
    
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {0,0};
    SetConsoleCursorPosition(hOut, pos);
    std::wstring output;
    wstring prewFgColor;
    wstring prewBgColor;
    for (int y = 0; y < SCREEN_H; ++y) {
        for (int x = 0; x < SCREEN_W; ++x) {
            const Glyph& fgG = fgLayer[y][x];
            const Glyph& bgG = bgLayer[y][x];

            wstring fgColor =  std::to_wstring(fgG.fg.r) + L";" + std::to_wstring(fgG.fg.g) + L";" + std::to_wstring(fgG.fg.b);
            wstring bgColor =  std::to_wstring(bgG.bg.r) + L";" + std::to_wstring(bgG.bg.g) + L";" + std::to_wstring(bgG.bg.b);

            if (fgColor != prewFgColor){
                output += L"\033[38;2;" + fgColor + L"m";
                prewFgColor = fgColor;
            }
            if (bgColor != prewBgColor){
                output += L"\033[48;2;" + bgColor + L"m";
                prewBgColor = bgColor;
            }
            output += fgG.ch;
        }
        output += L"\033[0m\n";
    }
    DWORD written;
    WriteConsoleW(hOut, output.c_str(), output.size(), &written, nullptr);
}

// Настройка консоли Windows для UTF-8 и ANSI
void setupConsole() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    _setmode(_fileno(stdout), _O_U16TEXT); // <-- добавить
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, mode);
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(hOut, &ci);
    ci.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &ci);
}

int main() {
    setupConsole();
    initScreen();

    int startX = 150;
    int startY = -2;
    int xOffset = 8;
    int yOffset = 2;
    int fov = 20;
    const int levelLenght = 34;
    const int levelWidth = 6;

    levelTile level[levelLenght][levelWidth] = {
        {levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()}
        ,{levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()}
        ,{levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()}
        ,{levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(2),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}             //wall here
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(2, false, 2),levelTile(2)}   //wall here
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()}
        ,{levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()}
        ,{levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()}
};

    int iterator = 0;
    int shipX = 20;
    int shipY = 40;
    int j;
     

    while (true){
        
        int currentStart = (levelLenght - fov) - iterator;

        
        // for (int pixelOffest = 0; pixelOffest < 5 ; ++pixelOffest){
        //     initScreen();
        //     for (int y = currentStart; y < currentStart + fov; ++y){
        //         for(int x = 0; x < levelWidth; ++x){
        //             j = y - currentStart;
                    
        //             int tileY = startY + (j * yOffset) + (x * yOffset) + pixelOffest ;
        //             int tileX = startX - (j * xOffset) + ((x * xOffset)) - pixelOffest * 2;

        //             if(level[y][x].type == 1)
        //                 drawSprite(tileX, tileY, tile);
                    
        //             else if(level[y][x].type == 2){
        //                 for(int height = 0; height < level[y][x].height; ++height){
        //                     drawSprite(tileX, tileY - (height * 4), wall);
        //                 }
        //             }


        //             drawSprite(shipX, shipY, ship);
        //             level[shipY][shipX].isShip = false;
        //             if ((shipY < levelLenght - 1) || (shipY > 0))
        //                 level[shipY - 1][shipX].isShip = true;
                        
                    
        //         }
        //     }
            
        //     render();
        // }
        drawSprite(0, 0, tile);
        drawSprite(16, 0, tile);
        render();
        
        // iterator++;
        // if (currentStart <= 0) fov--;
        if (iterator >= levelLenght - fov) break;
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    

    // drawSprite(50, 35, ship);
    

    
    return 0;
}