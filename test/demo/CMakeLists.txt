aux_source_directory(. DEMO_SRC)

include_directories(${PROJECT_SOURCE_DIR}/include/demo)

add_library(demolib ${DEMO_SRC})