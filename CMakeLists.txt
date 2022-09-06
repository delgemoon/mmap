project(connector)
cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)

include(dependencies/cmake/ForwardCMakeVars.cmake)


file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dependencies)


set(CONAN_PROFILE "clang")


execute_process(
        COMMAND conan install ${CMAKE_CURRENT_SOURCE_DIR}/dependencies --profile ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/${CONAN_PROFILE} --build=missing
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dependencies
        RESULT_VARIABLE conan_RESULT)


include(${CMAKE_BINARY_DIR}/dependencies/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)
include_directories(${CONAN_INCLUDE_DIRS})
include_directories(lib/kafka)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH};${CMAKE_BINARY_DIR}/dependencies)


find_package(Threads REQUIRED)
set(Boost_INSTALL_DIR ${t})
find_package(Boost COMPONENTS iostreams REQUIRED)
find_package(date REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(liburing REQUIRED)
find_package(argh REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(doctest REQUIRED)

link_libraries(liburing::liburing)

add_compile_definitions(BOOST_ASIO_HAS_IO_URING=1)
add_compile_definitions(SPDLOG_FMT_EXTERNAL=1)

add_subdirectory(lib)
add_subdirectory(apps)