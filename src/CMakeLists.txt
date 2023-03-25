aux_source_directory(. CUR_SRC)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(srclib ${CUR_SRC})

