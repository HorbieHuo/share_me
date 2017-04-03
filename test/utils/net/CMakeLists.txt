AUX_SOURCE_DIRECTORY(. SRC_LIST)
add_executable(socket_test ${SRC_LIST})

# IF(UNIX)
    TARGET_LINK_LIBRARIES(socket_test SSocket gtest)
# ELSEIF(WIN32)
#     TARGET_LINK_LIBRARIES(socket_test SSocket gtest)
# ENDIF(UNIX)