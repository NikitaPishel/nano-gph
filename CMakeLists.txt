cmake_minimum_required(VERSION 3.14)
project(ngph)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)

# Create a library of the core app 
add_library(ngph_lib STATIC
    ./src/grid.cpp
    ./src/texture.cpp
    ./src/canvas.cpp
    ./src/iotex.cpp
    ./src/gphUtil.cpp
)

# Set the public include directories for the library.
target_include_directories(ngph_lib PUBLIC
    ./include
)

# The main executable links to the library.
add_executable(ngph ./src/main.cpp)
target_link_libraries(ngph PRIVATE ngph_lib)

# Option to enable tests (OFF by default)
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    include(FetchContent)
    enable_testing()

    # Download and unpack gtest
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
    )

    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Add test subdirectory (tests folder must contain its own CMakeLists.txt)
    add_subdirectory(tests)
endif()