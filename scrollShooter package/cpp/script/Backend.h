#pragma once
#include <vector>
#include "sprites.h" 
#include <string> 

class Backend {
public:
    virtual ~Backend() = default;   

    virtual bool init(int screenWidth, int screenHeight) = 0;

    virtual void shutdown() = 0;

    virtual void present(std::vector<std::vector<Pixel>>& currentBuffer,
                     std::vector<std::vector<Pixel>>& previousBuffer) = 0;

    virtual int getKey() = 0;

    virtual bool shouldClose() { return false; }   

    virtual void drawCenterdText(std::string text) = 0;
};