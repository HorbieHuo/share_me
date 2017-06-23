INCLUDE_DIRECTORIES(${HEADER_OUTPUT_PATH})
MESSAGE(STATUS "ppp ${HEADER_OUTPUT_PATH}")
link_directories(${GTEST_ROOT})
AUX_SOURCE_DIRECTORY(. SRC_LIST)

AUX_SOURCE_DIRECTORY(. SRC_LIST)



ADD_LIBRARY(SLog STATIC ${SRC_LIST})
# TARGET_LINK_LIBRARIES(SLog pthread)
# IF(${UNITTEST})
    # add_executable(main main.cpp)
    # ADD_EXECUTABLE(log_unittest log_unittest.cpp)
    # TARGET_LINK_LIBRARIES(log_unittest SLog gtest gtest_main)
    # TARGET_LINK_LIBRARIES(socket_unittest gtest gtest_main)
# ENDIF()