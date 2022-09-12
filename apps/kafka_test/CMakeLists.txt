message("kafka build")
add_executable(kafka_ut main.spec.cpp endian.spec.cpp response_read.spec.cpp request_write.spec.cpp)
TARGET_LINK_LIBRARIES( kafka_ut doctest::doctest)