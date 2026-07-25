cmake_minimum_required(VERSION 3.16)
project(conback_native CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Собираем разделяемую библиотеку (DLL / SO)
add_library(conback_native SHARED
    c_api.cpp
    ConsoleBackend.cpp
)

# Если у вас есть другие .cpp файлы (например, uint4_t.cpp), добавьте их в список выше

if(WIN32)
    target_link_libraries(conback_native PRIVATE user32)
endif()