set(SRC_LIST main.cpp
             ut_skiplist.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${GTEST_PATH})

link_directories(${GTEST_LIB})

add_executable(ut ${SRC_LIST})
# target_link_directories(ut PRIVATE ${GTEST_PATH})

target_link_libraries(ut PRIVATE libgtest.a libgtest_main.a)