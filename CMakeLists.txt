cmake_minimum_required(VERSION 3.16)
set(THIS GB)

project(${THIS})

set(CMAKE_CXX_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_subdirectory(googletest)

set(Headers
    GB.hpp
)

set(Sources
    GB.cpp
    main.cpp
)


# Make roms directory if it doesn't exist
if (NOT EXISTS ${CMAKE_SOURCE_DIR}/roms)
  file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/roms)
  message("Created roms directory, please copy your roms into this directory and re-run cmake.")
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/roms DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(GameboyEmulator ${Sources} ${Headers})

add_library(${THIS} STATIC ${Sources} ${Headers})

target_link_libraries(GameboyEmulator ${SDL2_LIBRARIES})

add_subdirectory(test)


# --- auto-ignore build directory
if(NOT EXISTS ${PROJECT_BINARY_DIR}/.gitignore)
  file(WRITE ${PROJECT_BINARY_DIR}/.gitignore "*")
endif()