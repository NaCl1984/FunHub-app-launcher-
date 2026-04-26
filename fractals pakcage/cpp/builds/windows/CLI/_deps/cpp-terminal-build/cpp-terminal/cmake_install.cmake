# Install script for directory: D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/FractalApp")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-build/cpp-terminal/Debug/cpp-terminal.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-build/cpp-terminal/Release/cpp-terminal.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-build/cpp-terminal/MinSizeRel/cpp-terminal.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-build/cpp-terminal/RelWithDebInfo/cpp-terminal.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cpp-terminal" TYPE FILE FILES
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/args.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/buffer.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/color.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/cursor.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/event.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/exception.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/focus.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/input.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/iostream_initializer.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/iostream.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/key.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/mouse.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/options.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/position.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/prompt.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/screen.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/stream.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/style.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/size.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/terminal_impl.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/terminal_initializer.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/terminal.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/terminfo.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/tty.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/version.hpp"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src/cpp-terminal/window.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/cpp-terminal/cpp-terminalTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/cpp-terminal/cpp-terminalTargets.cmake"
         "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-build/cpp-terminal/CMakeFiles/Export/f914531f6191163e7a988aed981bcf11/cpp-terminalTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/cpp-terminal/cpp-terminalTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/cpp-terminal/cpp-terminalTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/cpp-terminal" TYPE FILE FILES "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-build/cpp-terminal/CMakeFiles/Export/f914531f6191163e7a988aed981bcf11/cpp-terminalTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/cpp-terminal" TYPE FILE FILES "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-build/cpp-terminal/CMakeFiles/Export/f914531f6191163e7a988aed981bcf11/cpp-terminalTargets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/cpp-terminal" TYPE FILE FILES "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-build/cpp-terminal/CMakeFiles/Export/f914531f6191163e7a988aed981bcf11/cpp-terminalTargets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/cpp-terminal" TYPE FILE FILES "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-build/cpp-terminal/CMakeFiles/Export/f914531f6191163e7a988aed981bcf11/cpp-terminalTargets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/cpp-terminal" TYPE FILE FILES "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-build/cpp-terminal/CMakeFiles/Export/f914531f6191163e7a988aed981bcf11/cpp-terminalTargets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/cpp-terminal" TYPE FILE FILES
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-build/cmake/cpp-terminalConfig.cmake"
    "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-build/cpp-terminal/cpp-terminalConfigVersion.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-build/cpp-terminal/private/cmake_install.cmake")

endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-build/cpp-terminal/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
