cmake_minimum_required(VERSION 3.15)          # Минимальная версия CMake
project(FractalApp)                           # Имя вашего проекта

set(CMAKE_CXX_STANDARD 17)                    # Используйте стандарт C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Укажите путь к папке, где лежит ваш код
set(SOURCES
    fractals.cpp
)

add_executable(FractalApp ${SOURCES})         # Создаём исполняемый файл

# Подключаем библиотеку cpp-terminal
# Это заставит CMake скачать и добавить её в проект автоматически
include(FetchContent)
FetchContent_Declare(
    cpp-terminal
    GIT_REPOSITORY https://github.com/jupyter-xeus/cpp-terminal.git
    GIT_TAG master
)
FetchContent_MakeAvailable(cpp-terminal)

target_link_libraries(FractalApp PRIVATE cpp-terminal)