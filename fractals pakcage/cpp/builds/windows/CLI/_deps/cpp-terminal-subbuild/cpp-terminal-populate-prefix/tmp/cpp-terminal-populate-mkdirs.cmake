# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src")
  file(MAKE_DIRECTORY "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-src")
endif()
file(MAKE_DIRECTORY
  "D:/code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-build"
  "D:/Code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-subbuild/cpp-terminal-populate-prefix"
  "D:/Code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-subbuild/cpp-terminal-populate-prefix/tmp"
  "D:/Code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-subbuild/cpp-terminal-populate-prefix/src/cpp-terminal-populate-stamp"
  "D:/Code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-subbuild/cpp-terminal-populate-prefix/src"
  "D:/Code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-subbuild/cpp-terminal-populate-prefix/src/cpp-terminal-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-subbuild/cpp-terminal-populate-prefix/src/cpp-terminal-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Code/funHub/fractals pakcage/cpp/builds/windows/CLI/_deps/cpp-terminal-subbuild/cpp-terminal-populate-prefix/src/cpp-terminal-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
