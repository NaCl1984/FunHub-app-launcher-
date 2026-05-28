#include <iostream>
#include <vector>
#include <string>
#include "sprites.h"   // ваш сгенерированный файл
#include <fcntl.h>
// #include <io.h>
#include <thread>
#include <chrono>
#include "levels.h"
#include "ConsoleBackend.h"
#include <unordered_map>
#include <cmath>
#include <algorithm>

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


const uint8_t SCREEN_W = 80;
const uint8_t SCREEN_H = 100;
int8_t state = 1;

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
    int heightStep = 7;
    int maxHeight = 3;

    void move(char dir){
        if (dir == 'r'){
            if(x + 2 <= 47){
            x += 2;
            y += 1;}
        }
        else if (dir == 'l'){
            if(x - 2 >= 6){
            x -= 2;
            y -= 1;}
        }
        else if (dir == 'u' && (height + 1) <= maxHeight){
            height += 1;
            y -= heightStep;
        }
        else if (dir == 'd' && (height - 1) >= 1){
            height -= 1;
            y += heightStep;
        }


        if(height > maxHeight)
            height = maxHeight;
        else if (height < 1)
            height = 1;
    }

};

struct Bullet {
    int x, y;          
    int dir;    
    bool active;
    uint8_t height;
};
std::vector<Bullet> bullets;

struct RenderOrder{
    bool isShip = false;
    bool isBullet = false;
    bool isEnemy = false;
};

struct Enemy{
    int x, y;
    uint8_t expFrame = 0;
    uint8_t bulletColldown = 0;
};

struct PairHash {
    std::size_t operator()(const std::pair<int,int>& p) const {
        return p.first * 10000 + p.second; // простой вариант
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
bool gameLoop(const levelTile (&level)[Rows][Cols], Backend& backend){
    const Pixel debugPixel[1][1] = {{{{1,255,3}, false}}};
    
    uint8_t frameTime = 60;

    Ship player;
    player.x = 10;
    player.y = 75 - (120 - SCREEN_H);

    Coords ancorPointShip(16, 4);
    
    int keyCode;
    const uint8_t xOffset = 8;
    const uint8_t yOffset = 4;
    int startX = 0;
    int startY = 80 - (120 - SCREEN_H);
    int pixelOffest = 0;
    int j;

    uint8_t bulletColldown = 0;
    uint8_t selfExpFrame = 0;
    uint8_t animationFrame = 0;

    Coords logicShipCoords = screenToLogic(player.x + ancorPointShip.x, player.y + ancorPointShip.y, startX, startY, Rows, 0);
    int logicShipX = logicShipCoords.x;
    int logicShipY = logicShipCoords.y;

    RenderOrder renderOrder[Rows][Cols];
    for(int y = 0; y < Rows; ++y){
        for(int x = 0; x < Cols; ++x){
            renderOrder[y][x] = RenderOrder();
        }
    }

    renderOrder[logicShipY][logicShipX].isShip = true;  
    renderOrder[logicShipY + 1][logicShipX].isShip = true; 
    renderOrder[logicShipY + 2][logicShipX].isShip = true; 

    std::unordered_map<std::pair<int,int>, Enemy, PairHash> enemys;

    for(int y = 0; y < Rows; ++y){
        for(int x = 0; x < Cols; ++x){
            if(level[y][x].type == 3){
                Enemy e;
                e.x = x; e.y = y;
                enemys[{x, y}] = e;
            }
        }
    }

    bool isMoving = true;
    bool isDefeat = false;

    while (true){
        auto frameStart = std::chrono::steady_clock::now();
        initScreen();
        
        //обновляем пули
        for (auto& b : bullets) {
            b.x += b.dir * 4;
            b.y -= b.dir * 2;
            // проверка выхода за пределы экрана
            if (b.x < -50 || b.x >= SCREEN_W + 50 || b.y < -50 || b.y >= SCREEN_H + 50)
                b.active = false;
        }
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.active; }), bullets.end());

        for (auto& b : bullets) {
            Coords bulletLogicCoords = screenToLogic(b.x + 11, b.y + (player.heightStep * b.height) + 6, startX, startY, Rows, pixelOffest);
            if (level[bulletLogicCoords.y][bulletLogicCoords.x].type == 3 && b.dir == 1 && level[bulletLogicCoords.y][bulletLogicCoords.x].height == b.height){
                auto it = enemys.find({bulletLogicCoords.x, bulletLogicCoords.y});
                if (it != enemys.end()) {
                    it->second.expFrame = 1;
                    b.active = false;
                }
            }
        }

        for(auto& [coords, e] : enemys) {
            if(e.expFrame == 0){
                if(e.bulletColldown == 0){
                    j = e.y - Rows;
                    int tileY = startY + (j * yOffset) + (e.x * yOffset) + pixelOffest - (player.heightStep * level[e.y][e.x].height);
                    int tileX = startX - (j * xOffset) + (e.x * xOffset) - pixelOffest * 2;
                    Bullet b;
                    b.x = tileX - 8; 
                    b.y = tileY + 4; 
                    b.dir = -1;          
                    b.active = true;
                    b.height = level[e.y][e.x].height;
                    bullets.push_back(b);
                    e.bulletColldown = 30;
                }
                else --e.bulletColldown;
            }
        }

        if(selfExpFrame < 20){
            //первый слой
            for (int y = 0; y < Rows; ++y){
                for(int x = 0; x < Cols; ++x){
                    j = y - Rows;
                    
                    int tileY = startY + (j * yOffset) + (x * yOffset) + pixelOffest ;
                    int tileX = startX - (j * xOffset) + (x * xOffset) - pixelOffest * 2;

                    if(level[y][x].type == 1 || level[y][x].type == 3)
                        drawSprite(tileX, tileY, tile);

                }
            }

            for (auto& b : bullets) {
                drawSprite(b.x, b.y + (b.height * player.heightStep), bulletShadow);
            }

            //второй слой
            for (int y = 0; y < Rows; ++y){
                for(int x = 0; x < Cols; ++x){
                    j = y - Rows;
                    
                    int tileY = startY + (j * yOffset) + (x * yOffset) + pixelOffest;
                    int tileX = startX - (j * xOffset) + (x * xOffset) - pixelOffest * 2;

                    logicShipCoords = screenToLogic(player.x + ancorPointShip.x, player.y + ancorPointShip.y + (player.height * player.heightStep), startX, startY, Rows, pixelOffest);
                    
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
                
                    if (renderOrder[y][x].isShip && !isDefeat){
                        drawSprite(player.x, player.y + (player.height * player.heightStep), shipShadow);
                    }
                        

                    if(level[y][x].type == 2 && level[y][x].height < player.height){
                        for(int height = 0; height < level[y][x].height; ++height){
                            drawSprite(tileX, tileY - (height * 8), wall);
                        }
                    }

                    if(level[y][x].type == 3 && level[y][x].height < player.height){
                        auto it = enemys.find({x, y});
                        if (it != enemys.end()) {
                            Enemy& e = it->second;
                            drawSprite(tileX, tileY, enemyShadow);   
                            if(e.expFrame == 0) {
                                int8_t animationOffset = 0;
                                if(animationFrame == 0) animationOffset = 0;
                                else if (animationFrame == 5) animationOffset = 1;
                                else if(animationFrame == 10) animationOffset = 0;
                                else if(animationFrame == 15) animationOffset = -1;
                                
                                drawSprite(tileX, tileY - (player.heightStep * level[y][x].height) + animationOffset, enemy);  

                            }
                            else{
                                if(e.expFrame >= 1 && e.expFrame < 5) {
                                    drawSprite(tileX, tileY - (level[y][x].height * player.heightStep), expFrame1); 
                                    ++e.expFrame;
                                }
                                else if(e.expFrame >= 5 && e.expFrame < 10) {
                                    drawSprite(tileX, tileY - (level[y][x].height * player.heightStep), expFrame2);
                                    ++e.expFrame;
                                }
                                else if(e.expFrame >= 10 && e.expFrame < 15) {
                                    drawSprite(tileX, tileY - (level[y][x].height * player.heightStep), expFrame3); 
                                    ++e.expFrame;
                                }
                                else if (e.expFrame >= 15 && e.expFrame < 20) e.expFrame = 20;
                            }
                        }
                    }
                    
                    if (renderOrder[y][x].isShip && !isDefeat){   
                        drawSprite(player.x, player.y, ship);
                    }
                    
                    if(level[y][x].type == 2  && level[y][x].height >= player.height){
                        for(int height = 0; height < level[y][x].height; ++height){
                            drawSprite(tileX, tileY - (height * 8), wall);
                        }
                    }

                    if(level[y][x].type == 3 && level[y][x].height >= player.height){
                        auto it = enemys.find({x, y});
                        if (it != enemys.end()) {
                            Enemy& e = it->second;
                            drawSprite(tileX, tileY, enemyShadow);   
                            if(e.expFrame == 0){
                                int8_t animationOffset = 0;
                                if(animationFrame >= 0 && animationFrame < 5) animationOffset = 0;
                                else if (animationFrame >= 5 && animationFrame < 10) animationOffset = 1;
                                else if(animationFrame >= 10 && animationFrame < 15) animationOffset = 0;
                                else if(animationFrame >= 15) animationOffset = -1;
                                
                                drawSprite(tileX, tileY - (player.heightStep * level[y][x].height) + animationOffset, enemy); 
                            }
                            else{
                                if(e.expFrame >= 1 && e.expFrame < 5) {
                                    drawSprite(tileX, tileY - (level[y][x].height * player.heightStep), expFrame1); 
                                    ++e.expFrame;
                                }
                                else if(e.expFrame >= 5 && e.expFrame < 10) {
                                    drawSprite(tileX, tileY - (level[y][x].height * player.heightStep), expFrame2);
                                    ++e.expFrame;
                                }
                                else if(e.expFrame >= 10 && e.expFrame < 15) {
                                    drawSprite(tileX, tileY - (level[y][x].height * player.heightStep), expFrame3); 
                                    ++e.expFrame;
                                }
                                else if (e.expFrame >= 15 && e.expFrame < 20) e.expFrame = 20;
                            }
                        }
                            
                    }               
                }
            }

            
        }        

        //draw self explosion
            if(selfExpFrame >= 1 && selfExpFrame < 5) {
                drawSprite(player.x, player.y, expFrame1); 
                ++selfExpFrame;
            }
            else if(selfExpFrame >= 5 && selfExpFrame < 10) {
                drawSprite(player.x, player.y, expFrame2);
                ++selfExpFrame;
            }
            else if(selfExpFrame >= 10 && selfExpFrame < 15) {
                drawSprite(player.x, player.y, expFrame3); 
                ++selfExpFrame;
            }
            else if (selfExpFrame >= 15 && selfExpFrame < 20) ++selfExpFrame;

            //draw bullets
            for (auto& b : bullets) {
                if(b.dir == 1) drawSprite(b.x, b.y, playerBullet);
                else if(b.dir == -1) drawSprite(b.x, b.y, enemyBullet);
            }

        //delete enemys
        for(auto it = enemys.begin(); it != enemys.end(); ) {
            if(it->second.expFrame >= 20) {
                it = enemys.erase(it);  
            } else {
                ++it;
                }
        }

        if(isDefeat && selfExpFrame >= 20){
            initScreen();
            backend.present(screenBuffer, prewFrame);
            backend.drawCenterdText("Game Over!\n Press R to restart");
        }


        keyCode = backend.getKey();

        //contorl
        if (keyCode == 27) {
            return false;
        }
        else if (keyCode == 32 && bulletColldown == 0 && !isDefeat) {
            Bullet b;
            b.x = player.x + 10; 
            b.y = player.y - 5; 
            b.dir = 1;          
            b.active = true;
            b.height = player.height;
            bullets.push_back(b);
            bulletColldown = 5;
        }
        else if(isMoving){
            if (keyCode == 1000){
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
        }
        else if (keyCode == 'r' && isDefeat){
            isDefeat = false;
            isMoving = true;
            pixelOffest = 0;
            player.x = 10;
            player.y = 75 - (120 - SCREEN_H);
            player.height = 1;
            selfExpFrame = 0;
            enemys.clear();
            bullets.clear();
            for(int y = 0; y < Rows; ++y){
                for(int x = 0; x < Cols; ++x){
                    if(level[y][x].type == 3){
                        Enemy e;
                        e.x = x; e.y = y;
                        enemys[{x, y}] = e;
                    }
                }
            }
        }
        

        //collision section
        Coords shipHitboxUpperScreenPoint(player.x + 13, player.y + (player.height - 1) * player.heightStep + 7);
        Coords shipHitboxLowerScreenPoint(player.x + 5, player.y + (player.height - 1) * player.heightStep + 8);
        
        Coords shipHitboxUpperLogicPoint = screenToLogic(shipHitboxUpperScreenPoint.x, shipHitboxUpperScreenPoint.y + player.heightStep, startX, startY, Rows, pixelOffest);
        Coords shipHitboxLowerLogicPoint = screenToLogic(shipHitboxLowerScreenPoint.x, shipHitboxLowerScreenPoint.y + player.heightStep, startX, startY, Rows, pixelOffest);

        if (!isDefeat){
            if(level[shipHitboxUpperLogicPoint.y][shipHitboxUpperLogicPoint.x].type == 2 || level[shipHitboxUpperLogicPoint.y + 1][shipHitboxUpperLogicPoint.x].type == 2 || level[shipHitboxUpperLogicPoint.y - 1][shipHitboxUpperLogicPoint.x].type == 2 ||
                level[shipHitboxUpperLogicPoint.y][shipHitboxUpperLogicPoint.x].type == 3 || level[shipHitboxUpperLogicPoint.y + 1][shipHitboxUpperLogicPoint.x].type == 3 || level[shipHitboxUpperLogicPoint.y - 1][shipHitboxUpperLogicPoint.x].type == 3){
                for(int i = -1; i < 2; ++i){          
                    int x = shipHitboxUpperLogicPoint.x;
                    int y = shipHitboxUpperLogicPoint.y + i;
                    int j = y - Rows;
                    
                    Coords wallMinPoit(startX - (j * xOffset) + (x * xOffset) - (pixelOffest * 2) + 3, startY + (j * yOffset) + (x * yOffset) + pixelOffest + 13);
                    Coords wallMaxPoit(startX - (j * xOffset) + (x * xOffset) - (pixelOffest * 2) + 13, startY + (j * yOffset) + (x * yOffset) + pixelOffest + 3);

                    if(((wallMinPoit.x <= (shipHitboxUpperScreenPoint.x) && (shipHitboxUpperScreenPoint.x) <= wallMaxPoit.x) && (wallMinPoit.y >= (shipHitboxUpperScreenPoint.y) 
                        && (shipHitboxUpperScreenPoint.y) >= wallMaxPoit.y) && (level[y][x].type == 2) && level[y][x].height >= player.height)){
                        isMoving = false;
                        isDefeat = true;
                        selfExpFrame = 1;
                    }
                    else if(((wallMinPoit.x <= (shipHitboxUpperScreenPoint.x) && (shipHitboxUpperScreenPoint.x) <= wallMaxPoit.x) && (wallMinPoit.y >= (shipHitboxUpperScreenPoint.y) 
                        && (shipHitboxUpperScreenPoint.y) >= wallMaxPoit.y) && (level[y][x].type == 3) && level[y][x].height == player.height)){
                        auto it_enemy = enemys.find({x, y});
                        if (it_enemy != enemys.end() && it_enemy->second.expFrame == 0) {
                            isMoving = false;
                            isDefeat = true;
                            selfExpFrame = 1;
                        }
                    }
                }
            }

            if(level[shipHitboxLowerLogicPoint.y][shipHitboxLowerLogicPoint.x].type == 2 || level[shipHitboxLowerLogicPoint.y + 1][shipHitboxLowerLogicPoint.x].type == 2 || level[shipHitboxLowerLogicPoint.y - 1][shipHitboxLowerLogicPoint.x].type == 2 ||
                level[shipHitboxLowerLogicPoint.y][shipHitboxLowerLogicPoint.x].type == 3 || level[shipHitboxLowerLogicPoint.y + 1][shipHitboxLowerLogicPoint.x].type == 3 || level[shipHitboxLowerLogicPoint.y - 1][shipHitboxLowerLogicPoint.x].type == 3){
                for(int i = -1; i < 2; ++i){          
                    int x = shipHitboxLowerLogicPoint.x;
                    int y = shipHitboxLowerLogicPoint.y + i;
                    int j = y - Rows;
                    
                    Coords wallMinPoit(startX - (j * xOffset) + (x * xOffset) - (pixelOffest * 2) + 3, startY + (j * yOffset) + (x * yOffset) + pixelOffest + 13);
                    Coords wallMaxPoit(startX - (j * xOffset) + (x * xOffset) - (pixelOffest * 2) + 13, startY + (j * yOffset) + (x * yOffset) + pixelOffest + 3);

                    if((wallMinPoit.x <= (shipHitboxLowerScreenPoint.x) && (shipHitboxLowerScreenPoint.x) <= wallMaxPoit.x) && (wallMinPoit.y >= (shipHitboxLowerScreenPoint.y) 
                        && (shipHitboxLowerScreenPoint.y) >= wallMaxPoit.y) && (level[y][x].type == 2)  && level[y][x].height >= player.height){
                        isMoving = false;
                        isDefeat = true;
                        selfExpFrame = 1;
                    }
                    else if((wallMinPoit.x <= (shipHitboxLowerScreenPoint.x) && (shipHitboxLowerScreenPoint.x) <= wallMaxPoit.x) && (wallMinPoit.y >= (shipHitboxLowerScreenPoint.y) 
                        && (shipHitboxLowerScreenPoint.y) >= wallMaxPoit.y) && (level[y][x].type == 3)  && level[y][x].height == player.height){
                        auto it_enemy = enemys.find({x, y});
                        if (it_enemy != enemys.end() && it_enemy->second.expFrame == 0) {
                            isMoving = false;
                            isDefeat = true;
                            selfExpFrame = 1;
                        }
                    }
                }
            }

            //bullet collision

            int playerLeft   = shipHitboxLowerScreenPoint.x;  
            int playerRight  = shipHitboxUpperScreenPoint.x;   
            int playerTop    = shipHitboxUpperScreenPoint.y;  
            int playerBottom = shipHitboxLowerScreenPoint.y;   
            

            for (auto& b : bullets) {
                if (b.dir == -1 && b.active && b.height == player.height) {
                    int bulletLeft   = b.x + 4;
                    int bulletRight  = b.x + 11 ;
                    int bulletTop    = b.y + 6 + (b.height * player.heightStep);
                    int bulletBottom = b.y + 10 + (b.height * player.heightStep);

                    if (playerLeft < bulletRight && playerRight > bulletLeft &&
                        playerTop < bulletBottom && playerBottom > bulletTop) {
                        isDefeat = true;
                        isMoving = false;
                        selfExpFrame = 1;
                        b.active = false; 
                        break;
                    }
                }
            }
        }      

        if(animationFrame < 20) ++animationFrame;
        else animationFrame = 0;

        if(isMoving) ++pixelOffest;
        if(pixelOffest >= Rows * 4) return true; //сделать нормально завершение уровня;

        if(bulletColldown > 0) --bulletColldown;

        

        if(selfExpFrame < 20){
            cout << "↑/↓/←/→ – move, Space - shoot, ESC - quit";
            backend.present(screenBuffer, prewFrame);
        }
        auto frameEnd = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();
        int sleepTime = frameTime - elapsed;
        if (sleepTime > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        }
    }
}

void mainMenuLoop(){

}

int main() {
    ConsoleBackend backend;
    
    
    if (!backend.init(SCREEN_W, SCREEN_H)) {
        return 1;
    } 

    std::ios_base::sync_with_stdio(false);

    while (true){
        if (state == 0){
            cout << "\033[0m" << "\x1b[2J";
            break;
        }
        else if (state == 1){
            cout << "\033[0m" << "\x1b[2J";
            initScreen();
            backend.present(screenBuffer, prewFrame);
            backend.drawCenterdText("Gnorp\nPress enter to start...");
            while (true){
                int key = backend.getKey();

                if(key == 13){ state = 2; break;}
                else if(key == 27){state = 0; break;}
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        
        else if(state == 2){
            cout << "\033[0m" << "\x1b[2J";
            initScreen();
            backend.present(screenBuffer, prewFrame);
            backend.drawCenterdText("Level 1");
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            if (gameLoop(level1, backend)) {
                initScreen();
                backend.present(screenBuffer, prewFrame);
                backend.drawCenterdText("You win!\nNext level...");
                ++state;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
            else state = 1;
            
        }
        else if(state == 3){
            cout << "\033[0m" << "\x1b[2J";
            initScreen();
            backend.present(screenBuffer, prewFrame);
            backend.drawCenterdText("Level 2");
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            if (gameLoop(level2, backend)) {
                initScreen();
                backend.present(screenBuffer, prewFrame);
                backend.drawCenterdText("You win!\nThanks for playing\n Press enter to go back to main menu");
                state = 1;
                while (true){
                    int key = backend.getKey();

                    if(key == 13){ state = 1; break;}
                    else if(key == 27){state = 0; break;}
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            }
            else state = 1;
        }
    }
   

    backend.shutdown();
    return 0;
}