#pragma once
#include <vector>
#include "sprites.h"   // для Pixel

class Backend {
public:
    virtual ~Backend() = default;   // виртуальный деструктор – обязателен!

    // Инициализация бэкенда (размеры экрана в символах)
    virtual bool init(int screenWidth, int screenHeight) = 0;

    // Завершение работы (освобождение ресурсов, восстановление консоли и т.д.)
    virtual void shutdown() = 0;

    // Вывод текущего буфера на экран (сравнивая с предыдущим для оптимизации)
    virtual void present(std::vector<std::vector<Pixel>>& currentBuffer,
                     std::vector<std::vector<Pixel>>& previousBuffer) = 0;

    // Получить код нажатой клавиши (0 – нет, 27 – ESC, 1000-1003 – стрелки и т.д.)
    virtual int getKey() = 0;

    // Для оконной версии: закрыто ли окно? В консоли всегда false (или по ESC)
    virtual bool shouldClose() { return false; }   // не обязательно чисто виртуальный
};