cmake_minimum_required(VERSION 3.14)
project(ngph VERSION 2.3)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(NGPH_IS_TOP_LEVEL ON)
else()
    set(NGPH_IS_TOP_LEVEL OFF)
endif()

# Let user choose between static/shared libs (default: shared)
option(BUILD_SHARED_LIBS "Build shared libraries (.so) instead of static (.a)" ON)
option(NGPH_BUILD_DEMO "Build the ngph_demo executable" ${NGPH_IS_TOP_LEVEL})

# Create a library of the core app
add_library(ngph
    ./src/grid.cpp
    ./src/texture.cpp
    ./src/canvas.cpp
    ./src/iotex.cpp
    ./src/gphUtil.cpp
    ./src/colors.cpp
)

# Set public include directories for the library
target_include_directories(ngph
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if(NGPH_BUILD_DEMO)
    add_executable(ngph_demo ./src/main.cpp)
    target_link_libraries(ngph_demo PRIVATE ngph)
endif()

# Option to enable tests (change cache to OFF by default)
set(NGPH_BUILD_TESTS OFF CACHE BOOL "")

if(NGPH_BUILD_TESTS)
    include(FetchContent)
    enable_testing()

    # Fetch gtest
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
    )

    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Add test subdirectory
    add_subdirectory(tests)
endif()

# Build library
# Install the library (static and/or shared)
install(TARGETS ngph
    EXPORT ngphTargets
    ARCHIVE DESTINATION lib  # for static lib
    LIBRARY DESTINATION lib  # for shared
)

# Install public headers
install(DIRECTORY include/ DESTINATION include)

# Export CMake targets for other projects
install(EXPORT ngphTargets
    FILE ngphTargets.cmake
    DESTINATION lib/cmake/ngph
)

# Generate a Config file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ngphConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ngphConfig.cmake"
    INSTALL_DESTINATION lib/cmake/ngph
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ngphConfig.cmake"
        DESTINATION lib/cmake/ngph)
