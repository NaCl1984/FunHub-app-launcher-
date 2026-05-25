cmake_minimum_required(VERSION 3.15)          # Минимальная версия CMake
project(Gnorp)                           # Имя вашего проекта

set(CMAKE_CXX_STANDARD 17)                    # Используйте стандарт C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Укажите путь к папке, где лежит ваш код
set(SOURCES
    main.cpp
)



add_executable(Gnorp ${SOURCES} ConsoleBackend.cpp)         # Создаём исполняемый файл

if(MSVC)
    target_compile_options(Gnorp PRIVATE /utf-8)
endif()