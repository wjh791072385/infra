aux_source_directory(. TEST_SRC)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${GTEST_PATH})

link_directories(${GTEST_LIB})

add_subdirectory(demo)

add_executable(ut ${TEST_SRC})
target_link_libraries(ut pthread gtest)

if(APPLE) 
    target_link_libraries(ut    "-Wl,-force_load" 
                                demolib)
elseif(UNIX)
    target_link_libraries(ut    "-Wl,--whole-archive" 
                                demolib
                                "-Wl,--no-whole-archive")
else()
    target_link_libraries(ut    "/WHOLEARCHIVE:demolib")
endif()               