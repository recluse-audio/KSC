cmake_minimum_required(VERSION 3.15)
project(KSC VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CMAKE/SOURCES.cmake)
include(CMAKE/TESTS.cmake)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.1.0
)
FetchContent_MakeAvailable(Catch2)

add_executable(Tests
    ${KSC_SOURCES}
    ${TEST_SOURCES}
)

target_include_directories(Tests PRIVATE
    SOURCE/SHARED
)

target_link_libraries(Tests PRIVATE
    Catch2::Catch2WithMain
)
