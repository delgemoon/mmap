
add_executable( mmap_reader main.cpp)
target_link_libraries(mmap_reader http Boost::iostreams fmt::fmt)