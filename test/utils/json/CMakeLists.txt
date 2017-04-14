AUX_SOURCE_DIRECTORY(. SRC_LIST)
add_executable(json_unittest ${SRC_LIST})

TARGET_LINK_LIBRARIES(json_unittest SJson gtest)