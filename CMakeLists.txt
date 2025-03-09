cmake_minimum_required(VERSION 3.21)
project(climate-tracker C)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4 gmodule-export-2.0)

find_program(GLIB_COMPILE_RESOURCES glib-compile-resources REQUIRED)

add_executable(climate-tracker)
target_link_libraries(climate-tracker PRIVATE PkgConfig::GTK4)

if (CMAKE_C_COMPILER_ID STREQUAL "AppleClang" OR
    CMAKE_C_COMPILER_ID STREQUAL "Clang" OR
    CMAKE_C_COMPILER_ID STREQUAL "GNU")
  target_compile_options(climate-tracker PRIVATE -Wall -Wextra -Wpedantic $<$<CONFIG:Debug>:-g -Og> $<$<CONFIG:Release>:-O2 -DNDEBUG>)
endif()

add_subdirectory(src)
add_subdirectory(res)
