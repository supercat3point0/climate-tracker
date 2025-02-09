cmake_minimum_required(VERSION 3.21)
project(climate-tracker C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4 gmodule-export-2.0)

find_program(GLIB_COMPILE_RESOURCES glib-compile-resources REQUIRED)

add_executable(climate-tracker)
target_link_libraries(climate-tracker PRIVATE PkgConfig::GTK4)

if (MSVC)
  target_compile_options(climate-tracker PRIVATE /Wall)
else()
  target_compile_options(climate-tracker PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_subdirectory(src)
add_subdirectory(res)
