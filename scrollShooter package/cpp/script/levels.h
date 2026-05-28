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

// Уровень 100 тайлов (110 строк с буфером)
levelTile level1[104][6] = {
// 5 пустых строк сверху (финиш)
{levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()},
{levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()},

// ===== Активная зона: 100 строк =====
// Секция 1: ровный пол, вводные стены высотой 1
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(2,false,1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,1),levelTile(1)},

{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(2,false,2),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(1)},

// Секция 2: стены высотой 2 и враги на высоте 1 (заставляют лететь выше 1)
{levelTile(1),levelTile(2,false,2),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(1),levelTile(3,false,1),levelTile(1)},
{levelTile(3,false,1),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(1),levelTile(2,false,2)},

{levelTile(1),levelTile(2,false,2),levelTile(1),levelTile(1),levelTile(3,false,1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,1),levelTile(1),levelTile(1)},
{levelTile(2,false,2),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(3,false,1)},
{levelTile(1),levelTile(3,false,1),levelTile(2,false,2),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,1),levelTile(1)},

// Секция 3: стены высотой 1 и враги на высоте 3 (заставляют лететь ниже 3)
{levelTile(1),levelTile(2,false,1),levelTile(1),levelTile(3,false,3),levelTile(1),levelTile(1)},
{levelTile(3,false,3),levelTile(1),levelTile(2,false,1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(3,false,3),levelTile(2,false,1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(2,false,1),levelTile(1),levelTile(1),levelTile(3,false,3),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,1),levelTile(1),levelTile(3,false,3)},

{levelTile(2,false,1),levelTile(3,false,3),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,1),levelTile(3,false,3),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(3,false,3),levelTile(1),levelTile(1),levelTile(2,false,1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(3,false,3),levelTile(2,false,1)},
{levelTile(3,false,3),levelTile(2,false,1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},

// Секция 4: стены высотой 3 (полные блоки) только на отдельных колонках – проход обязателен на свободных
{levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(1)},
{levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1)},

{levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(2,false,3),levelTile(1)},
{levelTile(2,false,3),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,3)},
{levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1),levelTile(2,false,3)},
{levelTile(2,false,3),levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1)},

// Секция 5: смешанные высоты стен и врагов – вынужденное маневрирование
{levelTile(2,false,2),levelTile(3,false,3),levelTile(2,false,1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(2,false,2),levelTile(3,false,2),levelTile(2,false,1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,1),levelTile(3,false,3),levelTile(2,false,2),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,1),levelTile(2,false,3)},
{levelTile(2,false,3),levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,1),levelTile(3,false,2)},

{levelTile(3,false,2),levelTile(2,false,2),levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,3)},
{levelTile(2,false,1),levelTile(3,false,3),levelTile(2,false,2),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(2,false,1),levelTile(3,false,2),levelTile(2,false,3),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(3,false,1),levelTile(2,false,2),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,3),levelTile(2,false,1)},

// Повторим секции 2–5 ещё раз для заполнения 100 строк
{levelTile(1),levelTile(2,false,2),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(1),levelTile(3,false,1),levelTile(1)},
{levelTile(3,false,1),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(1),levelTile(2,false,2)},

{levelTile(1),levelTile(2,false,1),levelTile(1),levelTile(3,false,3),levelTile(1),levelTile(1)},
{levelTile(3,false,3),levelTile(1),levelTile(2,false,1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(3,false,3),levelTile(2,false,1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(2,false,1),levelTile(1),levelTile(1),levelTile(3,false,3),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,1),levelTile(1),levelTile(3,false,3)},

{levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(1)},
{levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1)},

{levelTile(2,false,2),levelTile(3,false,3),levelTile(2,false,1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(2,false,2),levelTile(3,false,2),levelTile(2,false,1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,1),levelTile(3,false,3),levelTile(2,false,2),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,1),levelTile(2,false,3)},
{levelTile(2,false,3),levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,1),levelTile(3,false,2)},

// Последние 5 строк активной зоны – простой пол для плавного завершения
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},

// 5 пустых строк снизу (старт)
{levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()},
{levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()}
};

// Уровень 150 тайлов (160 строк с буфером)
levelTile level2[154][6] = {
// 5 пустых строк сверху
{levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()},
{levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()},


// ===== Активная зона: 150 строк =====
// Начальный ровный участок (10 строк)
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},

{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},

// Секция A: стены высотой 1 и 2, враги на высоте 2
{levelTile(1),levelTile(2,false,1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,2),levelTile(1),levelTile(1)},
{levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,1)},

{levelTile(2,false,2),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(2,false,1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,2),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,1),levelTile(3,false,2),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,2)},

// Секция B: стены высотой 3 на части колонок + враги на высоте 1
{levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(1)},
{levelTile(3,false,1),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(2,false,3),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(2,false,3)},

{levelTile(2,false,3),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(3,false,1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(3,false,1)},
{levelTile(3,false,1),levelTile(1),levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(1)},

// Секция C: сложное чередование – стены высотой 2 и 3, враги на всех высотах
{levelTile(2,false,2),levelTile(3,false,3),levelTile(2,false,1),levelTile(3,false,2),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(2,false,3),levelTile(3,false,1),levelTile(2,false,2),levelTile(3,false,3),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,3),levelTile(2,false,1),levelTile(3,false,2)},
{levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(3,false,1),levelTile(2,false,2)},
{levelTile(2,false,1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,3)},

{levelTile(3,false,3),levelTile(2,false,1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,3)},
{levelTile(2,false,2),levelTile(3,false,3),levelTile(2,false,1),levelTile(3,false,2),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(2,false,2),levelTile(3,false,1),levelTile(2,false,3),levelTile(3,false,2),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(3,false,2),levelTile(2,false,1),levelTile(3,false,3)},
{levelTile(3,false,1),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,3),levelTile(2,false,1)},

// Повторяем секции A, B, C для заполнения 150 строк (всего потребуется ~15 повторов, но для компактности сделаем 3 блока)
// Блок 2 (A,B,C)
{levelTile(1),levelTile(2,false,1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,2),levelTile(1),levelTile(1)},
{levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,1)},

{levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(1)},
{levelTile(3,false,1),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(2,false,3),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(2,false,3)},

{levelTile(2,false,2),levelTile(3,false,3),levelTile(2,false,1),levelTile(3,false,2),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(2,false,3),levelTile(3,false,1),levelTile(2,false,2),levelTile(3,false,3),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,3),levelTile(2,false,1),levelTile(3,false,2)},
{levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(3,false,1),levelTile(2,false,2)},
{levelTile(2,false,1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,3)},

// Блок 3 (A,B,C)
{levelTile(1),levelTile(2,false,1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,2),levelTile(1),levelTile(1)},
{levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,1)},

{levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(1)},
{levelTile(3,false,1),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(2,false,3),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(2,false,3)},

{levelTile(2,false,2),levelTile(3,false,3),levelTile(2,false,1),levelTile(3,false,2),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(2,false,3),levelTile(3,false,1),levelTile(2,false,2),levelTile(3,false,3),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,3),levelTile(2,false,1),levelTile(3,false,2)},
{levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(3,false,1),levelTile(2,false,2)},
{levelTile(2,false,1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,3)},

// Блок 4 (A,B,C) – последний, чтобы достичь 150 строк
{levelTile(1),levelTile(2,false,1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,2),levelTile(1),levelTile(1)},
{levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,1)},

{levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(1)},
{levelTile(3,false,1),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(2,false,3),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(2,false,3),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(3,false,1),levelTile(1),levelTile(2,false,3)},

{levelTile(2,false,2),levelTile(3,false,3),levelTile(2,false,1),levelTile(3,false,2),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(2,false,3),levelTile(3,false,1),levelTile(2,false,2),levelTile(3,false,3),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,3),levelTile(2,false,1),levelTile(3,false,2)},
{levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,3),levelTile(3,false,1),levelTile(2,false,2)},
{levelTile(2,false,1),levelTile(3,false,2),levelTile(1),levelTile(1),levelTile(2,false,2),levelTile(3,false,3)},

// Финальные простые строки
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},
{levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1),levelTile(1)},

// 5 пустых строк снизу
{levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()},
{levelTile(),levelTile(),levelTile(),levelTile(),levelTile(),levelTile()}

};