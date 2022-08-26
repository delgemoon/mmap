message("http build")


add_library(http http.cpp)
target_include_directories(http PUBLIC ${CMAKE_SOURCE_DIR}/lib)
TARGET_LINK_LIBRARIES( http  PUBLIC liburing::liburing spdlog::spdlog Boost::json date::date Boost::headers Threads::Threads util)

add_executable(http_ut http.spec.cpp main.spec.cpp)
TARGET_LINK_LIBRARIES( http_ut  http doctest::doctest)