AUX_SOURCE_DIRECTORY(. SRC_LIST)
add_executable(socket_test ${SRC_LIST})

TARGET_LINK_LIBRARIES(socket_test SSocket gtest)