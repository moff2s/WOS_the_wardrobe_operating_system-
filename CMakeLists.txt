cmake_minimum_required(VERSION 3.16)
project(WardrobeSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Raylib paths
set(RAYLIB_DIR "C:/Users/User/raylib" CACHE PATH "Path to raylib installation")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${RAYLIB_DIR}/include
)

link_directories(
    ${RAYLIB_DIR}/lib
)

# Project sources
set(PROJECT_SOURCES
    main.cpp
    Sorting.cpp
    UndoStack.cpp
    WearQueue.cpp
)

add_executable(WardrobeSystem ${PROJECT_SOURCES})

target_link_libraries(WardrobeSystem PRIVATE raylib opengl32 gdi32 winmm)
