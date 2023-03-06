cmake_minimum_required(VERSION 2.9)

PROJECT(infra)



set(BASE_FLAGS            "-g -Wall -Werror")
set(SP_C_FLAGS            "-std=gnu99")
set(SP_C++_FLAGS          "-std=c++11")
set(SP_RELEASE_FLAGS      "-O2")
set(SP_DEBUG_FLAGS        "-O0")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    message(">>> x86_64")
    set(SP_PRO_FLAGS         "")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    message(">>> aarch64")
    set(SP_PRO_FLAGS         "")
ENDIF ()


if(CMAKE_BUILD_TYPE MATCHES "Release")
    message(">>> Release Mode")
    set(CMAKE_C_FLAGS       "${BASE_FALGS} ${SP_C_FLAGS}    ${SP_RELEASE_FLAGS} ${SP_PRO_FLAGS}")
    set(CMAKE_CXX_FLAGS     "${BASE_FALGS} ${SP_C++_FLAGS}  ${SP_RELEASE_FLAGS} ${SP_PRO_FLAGS}")
else()
    message(">>> Debug Mode")
    add_definitions(-D_DEBUG)
    set(CMAKE_C_FLAGS       "${BASE_FALGS} ${SP_C_FLAGS}    ${SP_DEBUG_FLAGS} ${SP_PRO_FLAGS}")
    set(CMAKE_CXX_FLAGS     "${BASE_FALGS} ${SP_C++_FLAGS}  ${SP_DEBUG_FLAGS} ${SP_PRO_FLAGS}")
endif()

message(STATUS ">>> Source Dir: " ${PROJECT_SOURCE_DIR})
message(STATUS ">>> Binary Dir: " ${PROJECT_BINARY_DIR})
message(STATUS ">>> C_FLAGS   : " ${CMAKE_C_FLAGS})
message(STATUS ">>> CXX_FLAGS : " ${CMAKE_CXX_FLAGS})

#*******************************************************************************
#*******************************************************************************

set(EXECUTABLE_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/bin)

#*******************************************************************************
#*******************************************************************************

set(GTEST_PATH /usr/local/include)
set(GTEST_LIB /usr/local/lib)

#*******************************************************************************
#*******************************************************************************

add_subdirectory(src)
add_subdirectory(test)