cmake_minimum_required(VERSION 3.16)

set(THIS GameboyEmulator)

project(${THIS})

set(CMAKE_CXX_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

add_subdirectory(googletest)

set(Headers
    GB.hpp
)

set(Sources
    GB.cpp
    main.cpp
)

add_library(${THIS} STATIC ${Sources} ${Headers})

add_subdirectory(test)


# --- auto-ignore build directory
if(NOT EXISTS ${PROJECT_BINARY_DIR}/.gitignore)
  file(WRITE ${PROJECT_BINARY_DIR}/.gitignore "*")
endif()