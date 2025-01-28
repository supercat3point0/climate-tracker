cmake_minimum_required(VERSION 3.21)
project(ClimateTracker C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

add_executable(ClimateTracker src/main.c)
set_target_properties(ClimateTracker PROPERTIES OUTPUT_NAME "climate-tracker")
target_link_libraries(ClimateTracker PRIVATE PkgConfig::GTK4)

if (MSVC)
  target_compile_options(ClimateTracker PRIVATE /Wall)
else()
  target_compile_options(ClimateTracker PRIVATE -Wall -Wextra -Wpedantic)
endif()
