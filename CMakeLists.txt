cmake_minimum_required(VERSION 3.21)
project(climate-tracker C)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4 gmodule-export-2.0)

find_program(GLIB_COMPILE_RESOURCES glib-compile-resources REQUIRED)

add_executable(climate-tracker)
target_link_libraries(climate-tracker PRIVATE PkgConfig::GTK4)


target_compile_options(climate-tracker PRIVATE "-Wall" "-Wextra" "-Wpedantic")
target_compile_options(climate-tracker PRIVATE "$<$<CONFIG:Debug>:-g3;-Og>")
target_compile_options(climate-tracker PRIVATE "$<$<CONFIG:Release>:-O2;-DNDEBUG>")
target_compile_options(climate-tracker PRIVATE "$<$<CONFIG:RelWithDebInfo>:-g3;-O2;-DNDEBUG>")
target_compile_options(climate-tracker PRIVATE "$<$<CONFIG:MinSizeRel>:-Oz;-DNDEBUG>")

add_subdirectory(src)
add_subdirectory(res)
