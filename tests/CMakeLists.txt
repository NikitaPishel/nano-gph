# Executable for graphics tests
add_executable(gphTest gphTest.cpp)
target_include_directories(gphTest PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(gphTest PRIVATE
    ngph_lib
    GTest::gtest_main
)
add_test(NAME gphTest COMMAND gphTest)