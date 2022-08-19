#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <vector>
using namespace boost::iostreams;
int main() {
    mapped_file_params params;
    params.path = "test.txt";
    params.flags = mapped_file::readwrite;
    params.new_file_size = 1ul << 10;
    boost::iostreams::stream<mapped_file_sink> writter{params};
    std::vector<std::string> v(2000);
    v[0] = "Tam is here";
    std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>(writter, "\n"));
    return 0;
}
