cmake_minimum_required(VERSION 3.18)
project(ClimateTracker C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

add_executable(ClimateTracker src/main.c)
set_target_properties(ClimateTracker PROPERTIES OUTPUT_NAME "climate-tracker")
target_link_libraries(ClimateTracker PRIVATE PkgConfig::GTK4)
