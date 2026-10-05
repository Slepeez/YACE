cmake_minimum_required(VERSION 3.24)
project(YACE C) 


set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic)


set(SDL_SHARED ON CACHE BOOL "Build shared library" FORCE)
set(SDL_STATIC OFF CACHE BOOL "Build static library" FORCE)
set(SDL_TEST OFF CACHE BOOL "Disable SDL test suite" FORCE)


add_subdirectory(extern/SDL3 )


set(SOURCES 
    src/main.c 
    src/chip8.c 
    src/display.c 
    src/keypad.c
)
add_executable(YACE ${SOURCES})


target_include_directories(YACE PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_link_libraries(YACE PRIVATE SDL3::SDL3-shared)


add_custom_command(TARGET YACE POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:SDL3::SDL3>"
    "$<TARGET_FILE_DIR:YACE>"
)
