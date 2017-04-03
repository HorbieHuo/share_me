AUX_SOURCE_DIRECTORY(. SRC_LIST)
add_executable(log_unittest ${SRC_LIST})

TARGET_LINK_LIBRARIES(log_unittest SLog)