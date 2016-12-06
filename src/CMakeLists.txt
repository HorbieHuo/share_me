cmake_minimum_required(VERSION 3.6)
project(share_me)

OPTION(GTEST "unittest, using googletest, default OFF" OFF)

IF(UNIX)
    ADD_DEFINITIONS(-D__unix__)
ELSEIF(WIN32)
    ADD_DEFINITIONS(-D_WIN32)
ENDIF(UNIX)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

ADD_SUBDIRECTORY(FileTransfer)
ADD_SUBDIRECTORY(utils)

INCLUDE_DIRECTORIES(./FileTransfer)

IF(${GTEST})
    IF(NOT ${GTEST_ROOT})
        SET(GTEST_ROOT ${PROJECT_SOURCE_DIR}/../external_dependencies/googletest)
    ENDIF()
    INCLUDE_DIRECTORIES(${GTEST_ROOT}/googletest/include)
    MESSAGE(STATUS "GTEST is ON")
ELSE()
    MESSAGE(STATUS "GTEST is OFF")
ENDIF()

add_executable(main main.cpp)

TARGET_LINK_LIBRARIES(main FileTransfer)
