#pragma once
#include "backend.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <termios.h>
#endif

struct TerminalSize {
    int rows;
    int cols;
};


class ConsoleBackend : public Backend {
public:
    ConsoleBackend();
    ~ConsoleBackend() override;

    bool init(int screenWidth, int screenHeight) override;
    void shutdown() override;
    void present(std::vector<std::vector<Pixel>>& currentBuffer,
                std::vector<std::vector<Pixel>>& previousBuffer) override;
    int getKey() override;

private:
    int width, height;
    // Для хранения предыдущих цветов при выводе (как в вашем render)
    int prewFgR = -1, prewFgG = -1, prewFgB = -1;
    int prewBgR = -1, prewBgG = -1, prewBgB = -1;

    TerminalSize getTerminalSize();

#ifdef __linux__
    static struct termios old_termios;
    static bool input_initialized;
#endif
};