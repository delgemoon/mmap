project(marvelsoft)
cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
include(ProcessorCount)
ProcessorCount(processors)

include(dependencies/cmake/ForwardCMakeVars.cmake)


file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dependencies)
execute_process(COMMAND conan remote list OUTPUT_VARIABLE conan_remotes RESULT_VARIABLE conan_RESULT)
if (conan_RESULT)
    message(WARN "[dependency] Conan not installed, running 'pip install conan'")
    execute_process(COMMAND pip install conan RESULT_VARIABLE conan_RESULT)
    if (conan_RESULT)
        message(FATAL_ERROR "[dependency] Couldn't install conan, are python and pip installed?.

    sudo dnf install python3 python3-pip python3-devel
")
    endif()
endif()


set(CONAN_PROFILE "clang")

file(LOCK ${CMAKE_CURRENT_SOURCE_DIR}/dependencies DIRECTORY GUARD PROCESS)


execute_process(
        COMMAND conan install ${CMAKE_CURRENT_SOURCE_DIR}/dependencies --profile ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/${CONAN_PROFILE} --build=missing
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dependencies
        RESULT_VARIABLE conan_RESULT)

file(LOCK ${CMAKE_CURRENT_SOURCE_DIR}/dependencies DIRECTORY RELEASE GUARD PROCESS)

if (conan_RESULT)
    message(INFO " If you want or need to build the conan dependencies from source, you will need the following packages:

    dnf install patch which nodejs python3-devel bash perl autoconf which automake -y
")
    message(FATAL_ERROR "[dependency] Conan failed.")
endif()

include(${CMAKE_BINARY_DIR}/dependencies/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)
include_directories(${CONAN_INCLUDE_DIRS})
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

link_libraries(liburing::liburing)

add_compile_definitions(BOOST_ASIO_HAS_IO_URING=1)
add_compile_definitions(SPDLOG_FMT_EXTERNAL=1)


add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}  Boost::iostreams)