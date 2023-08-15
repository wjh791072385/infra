aux_source_directory(. TEST_SRC)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${GTEST_PATH})

link_directories(${GTEST_LIB})

add_subdirectory(demo)

add_executable(ut ${TEST_SRC})
target_link_libraries(ut pthread gtest srclib)

if(APPLE) 
    target_link_libraries(ut    "-Wl,-force_load" 
                                demolib)
elseif(UNIX)
    target_link_libraries(ut    "-Wl,--whole-archive" 
                                demolib
                                "-Wl,--no-whole-archive")
else()
    target_link_libraries(ut    "/WHOLEARCHIVE:" demolib)
endif()    

add_test(
	NAME Testrwlock.performance 
	COMMAND ${PROJECT_BINARY_DIR}/bin/ut --gtest_filter="Testrwlock.performance"
)

add_test(
	NAME TestSkiplist.function 
	COMMAND ${PROJECT_BINARY_DIR}/bin/ut --gtest_filter="TestSkiplist.function"
)

add_test(
	NAME TestSkiplist.performance 
	COMMAND ${PROJECT_BINARY_DIR}/bin/ut --gtest_filter="TestSkiplist.function"
)