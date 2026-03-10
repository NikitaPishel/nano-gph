function(add_gph_test name source)
    add_executable(${name} ${source})
    target_include_directories(${name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
    )
    target_link_libraries(${name} PRIVATE
        ngph
        GTest::gtest_main
    )
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_gph_test(gridTest    gridTest.cpp)
add_gph_test(textureTest textureTest.cpp)
add_gph_test(iotexTest   iotexTest.cpp)
add_gph_test(canvasTest  canvasTest.cpp)
