#pragma once
#include <cstdint>

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

levelTile level1[34][6] = {
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
        ,{levelTile(1),levelTile(2),levelTile(1),levelTile(2),levelTile(1),levelTile(1)}              //wall hete
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)}
        ,{levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()}
        ,{levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()}
        ,{levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()}
};

