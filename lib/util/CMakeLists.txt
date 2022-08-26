message("util build")


add_library(util explainer.cpp)
target_include_directories(util PUBLIC ${CMAKE_SOURCE_DIR}/lib)
TARGET_LINK_LIBRARIES( util  PUBLIC liburing::liburing spdlog::spdlog Boost::json date::date Boost::headers Threads::Threads)

add_executable(util_ut explainer.spec.cpp main.spec.cpp)
TARGET_LINK_LIBRARIES( util_ut  util doctest::doctest)