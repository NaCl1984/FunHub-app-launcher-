#include <iostream>
#include <vector>
#include <string>
#include "sprites.h"   // ваш сгенерированный файл
#include <fcntl.h>
#include <io.h>
#include <thread>
#include <chrono>
#include "levels.h"
#include "ConsoleBackend.h"
#include "json.hpp"

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
using json = nlohmann::json;

const int SCREEN_W = 80;
const int SCREEN_H = 100;
uint8_t state = 2;

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

Coords screenToLogic(int screenX, int screenY, int startX, int startY, int levelLenght, int pixelOffset){
    int xOffset = 8;
    int yOffset = 4;
    
    float A = (screenY - startY + (levelLenght * yOffset) - pixelOffset)/yOffset;
    float B = (screenX - startX - (levelLenght * xOffset) + 2 * pixelOffset)/xOffset;

    int logicX = (int)round((A + B) / 2)  - 2;
    if (logicX > 5)
        logicX = 5;
    else if (logicX < 0)
        logicX = 0;

    int logicY = (int)round((A - B) / 2)  - 2;
    if(logicY > levelLenght - 1)
        logicY = levelLenght - 1;
    else if (logicY < 0)
        logicY = 0;

    Coords logicCoords(logicX, logicY);

    return logicCoords;
}

template<int Rows, int Cols>
void gameLoop(const levelTile (&level)[Rows][Cols], Backend& backend){
    Ship player;
    player.x = 10;
    player.y = 75 - (120 - SCREEN_H);

    Coords ancorPointShip(16, 4);
    
    int keyCode;
    int xOffset = 8;
    int yOffset = 4;
    int startX = 0;
    int startY = 80 - (120 - SCREEN_H);
    int pixelOffest = 0;

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

    bool isMoving = true;

    while (true){

        initScreen();
        
        
        //первый слой
        for (int y = 0; y < Rows; ++y){
            for(int x = 0; x < Cols; ++x){
                j = y - Rows;
                
                int tileY = startY + (j * yOffset) + (x * yOffset) + pixelOffest ;
                int tileX = startX - (j * xOffset) + (x * xOffset) - pixelOffest * 2;

                if(level[y][x].type == 1)
                    drawSprite(tileX, tileY, tile);

            }
        }


        //второй слой
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
            
                if (renderOrder[y][x].isShip)
                    drawSprite(player.x, player.y + (player.height * 5), shipShadow);

                if(level[y][x].type == 2 && level[y][x].height < player.height){
                    for(int height = 0; height < level[y][x].height; ++height){
                        drawSprite(tileX, tileY - (height * 8), wall);
                    }
                }
                
                if (renderOrder[y][x].isShip){
                    drawSprite(player.x, player.y, ship);
                }

                if(level[y][x].type == 2  && level[y][x].height >= player.height){
                    for(int height = 0; height < level[y][x].height; ++height){
                        drawSprite(tileX, tileY - (height * 8), wall);
                    }
                }
                
            }
        }

        keyCode = backend.getKey();

        if (keyCode == 27) {
            return;
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

        Coords shipHitboxUpperScreenPoint(player.x + 13, player.y + 3);
        Coords shipHitboxLowerScreenPoint(player.x + 3, player.y + 13);
        
        Coords shipHitboxUpperLogicPoint = screenToLogic(shipHitboxUpperScreenPoint.x, shipHitboxUpperScreenPoint.y + (player.height * 5), startX, startY, Rows, pixelOffest);
        Coords shipHitboxLowerLogicPoint = screenToLogic(shipHitboxUpperScreenPoint.x, player.y + 13 + (player.height * 5), startX, startY, Rows, pixelOffest);


        if(level[shipHitboxUpperLogicPoint.y][shipHitboxUpperLogicPoint.x].type == 2 || level[shipHitboxUpperLogicPoint.y + 1][shipHitboxUpperLogicPoint.x].type == 2 || level[shipHitboxUpperLogicPoint.y - 1][shipHitboxUpperLogicPoint.x].type == 2){
            for(int i = -1; i < 1; ++i){          
                int x = shipHitboxUpperLogicPoint.x;
                int y = shipHitboxUpperLogicPoint.y + i;
                int j = y - Rows;
                
                Coords wallMinPoit(startX - (j * xOffset) + (x * xOffset) - (pixelOffest * 2) + 3, startY + (j * yOffset) + (x * yOffset) + pixelOffest + 13);
                Coords wallMaxPoit(startX - (j * xOffset) + (x * xOffset) - (pixelOffest * 2) + 13, startY + (j * yOffset) + (x * yOffset) + pixelOffest + 3);

                if((wallMinPoit.x <= (shipHitboxUpperScreenPoint.x) && (shipHitboxUpperScreenPoint.x) <= wallMaxPoit.x) && (wallMinPoit.y >= (shipHitboxUpperScreenPoint.y) && (shipHitboxUpperScreenPoint.y) >= wallMaxPoit.y)){
                    isMoving = false;
                }
            }
        }

        if(level[shipHitboxLowerLogicPoint.y][shipHitboxLowerLogicPoint.x].type == 2 || level[shipHitboxLowerLogicPoint.y + 1][shipHitboxLowerLogicPoint.x].type == 2 || level[shipHitboxLowerLogicPoint.y - 1][shipHitboxLowerLogicPoint.x].type == 2){
            for(int i = -1; i < 1; ++i){          
                int x = shipHitboxLowerLogicPoint.x;
                int y = shipHitboxLowerLogicPoint.y + i;
                int j = y - Rows;
                
                Coords wallMinPoit(startX - (j * xOffset) + (x * xOffset) - (pixelOffest * 2) + 3, startY + (j * yOffset) + (x * yOffset) + pixelOffest + 13);
                Coords wallMaxPoit(startX - (j * xOffset) + (x * xOffset) - (pixelOffest * 2) + 13, startY + (j * yOffset) + (x * yOffset) + pixelOffest + 3);

                if((wallMinPoit.x <= (shipHitboxLowerScreenPoint.x) && (shipHitboxLowerScreenPoint.x) <= wallMaxPoit.x) && (wallMinPoit.y >= (shipHitboxLowerScreenPoint.y) && (shipHitboxLowerScreenPoint.y) >= wallMaxPoit.y)){
                    isMoving = false;
                }
            }
        }

        if(isMoving) ++pixelOffest;
        if(pixelOffest >= Rows * 4) break; //сделать нормально завершение уровня;

        backend.present(screenBuffer, prewFrame);
        std::this_thread::sleep_for(std::chrono::milliseconds(34)); 
    }
}

void mainMenuLoop(){

}

int main() {
    ConsoleBackend backend;
    
    
    if (!backend.init(SCREEN_W, SCREEN_H)) {
        return 1;
    } 

    initScreen();
    std::ios_base::sync_with_stdio(false);

    while (true){
        if (state == 1){
            mainMenuLoop();
        }
        else if (state == 0){
            break;
        }
        else if(state == 2){
            gameLoop(level1, backend);
            state = 0;
        }
    }
   

    backend.shutdown();
    return 0;
}