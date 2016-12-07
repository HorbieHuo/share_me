INCLUDE_DIRECTORIES(${HEADER_OUTPUT_PATH})
MESSAGE(STATUS "ppp ${HEADER_OUTPUT_PATH}")
link_directories(${GTEST_ROOT})
AUX_SOURCE_DIRECTORY(. SRC_LIST)

ADD_LIBRARY(SSocket STATIC socket.cpp)

IF(${UNITTEST})
    # add_executable(main main.cpp)
    ADD_EXECUTABLE(socket_unittest socket_unittest.cpp)
    TARGET_LINK_LIBRARIES(socket_unittest SSocket gtest gtest_main)
    # TARGET_LINK_LIBRARIES(socket_unittest gtest gtest_main)
ENDIF()