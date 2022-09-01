
add_executable( mmap_writer main.cpp)
target_link_libraries(mmap_writer http Boost::iostreams fmt::fmt)