cmake_minimum_required(VERSION 3.16)

set(THIS GBTests)

set(SOURCES
    GBTests.cpp
    )

add_executable(${THIS} ${SOURCES})

target_link_libraries(${THIS} PUBLIC
    gtest_main
    GameboyEmulator
)

add_test(
    NAME ${THIS}
    COMMAND ${THIS}
    )
