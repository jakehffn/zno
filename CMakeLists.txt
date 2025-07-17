project(zno)

cmake_minimum_required(VERSION 3.23)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)

# add_compile_definitions(
#     "TINYF_RENDER_GRID_CELL_SIZE=16"
#     "TINYF_MOUSE_INPUT_GRID_CELL_SIZE=16"
# )

add_executable(${PROJECT_NAME} src/main.c)

# if(MSVC)
#   target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
# else()
#   target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
# endif()

add_subdirectory(src)

install(TARGETS zno RUNTIME DESTINATION .)