aux_source_directory(. CUR_SRC)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/demo)

add_library(srclib ${CUR_SRC})

