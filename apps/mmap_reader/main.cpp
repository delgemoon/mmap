//
// Created by tammd on 8/29/22.
//

#include <boost/asio.hpp>
#include <boost/asio/stream_file.hpp>
#include <boost/interprocess/file_mapping.hpp>

#include <iostream>
#include <vector>

using namespace boost::asio;
using namespace std::literals;
using namespace boost::interprocess;
awaitable< void >
entry()
{
    auto        exec = co_await this_coro::executor;
    stream_file file(exec, "test.txt", stream_file::exclusive);
    if (!file.is_open())
    {
        std::cout << "file is not open" << std::endl;
    }

    steady_timer           timer(exec);
    boost::asio::streambuf buffer_;
    auto                   start_point     = std::chrono::system_clock::now();
    auto                   total_byte_read = 0;
    auto                   count_message   = 0;
    while (true)
    {
        std::string data;
        try
        {
            boost::system::error_code ec;
            auto bytes_read = co_await async_read_until(file, buffer_, "\n", redirect_error(use_awaitable, ec));
            if (ec == error::eof)
                break;
            std::cout << " error code = " << ec << " size_read = " << bytes_read << std::endl;
            buffer_.consume(bytes_read);
            total_byte_read += bytes_read;
            count_message++;
        }
        catch (std::exception exc)
        {
            std::cout << exc.what() << std::endl;
        }
        catch (...)
        {
            std::cout << "unknown failed" << std::endl;
        }
    }
    std::cout
        << "read " << count_message << " with " << total_byte_read / 1024 << " Kb"
        << " in  "
        << std::chrono::duration_cast< std::chrono::seconds >(std::chrono::system_clock::now() - start_point).count()
        << std::endl;
}

int
main()
{
    io_context io_ctx;
    co_spawn(io_ctx.get_executor(), entry(), detached);
    io_ctx.run();
}