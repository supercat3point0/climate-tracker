set(GRESOURCE_DEPENDENCIES CACHE INTERNAL "GResource dependencies")

execute_process(
  COMMAND "${GLIB_COMPILE_RESOURCES}" "--generate-dependencies" "resources.gresource.xml"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  OUTPUT_VARIABLE GRESOURCE_DEPENDENCIES
)

string(REPLACE "\n" ";" GRESOURCE_DEPENDENCIES "${GRESOURCE_DEPENDENCIES}")
list(POP_BACK GRESOURCE_DEPENDENCIES)

add_custom_command(
  COMMAND "${GLIB_COMPILE_RESOURCES}" "--generate" "--target=${CMAKE_CURRENT_BINARY_DIR}/resources.c" "resources.gresource.xml"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/resources.c"
  MAIN_DEPENDENCY "resources.gresource.xml"
  DEPENDS "${GRESOURCE_DEPENDENCIES}"
)

add_custom_target(compile-resources DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/resources.c")
add_dependencies(climate-tracker compile-resources)

target_sources(climate-tracker PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/resources.c")
set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/resources.c" PROPERTIES GENERATED TRUE)
