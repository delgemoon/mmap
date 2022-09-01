//
// Created by tammd on 8/29/22.
//

#include <boost/asio.hpp>
#include <boost/asio/stream_file.hpp>

#include <iostream>
#include <vector>

using namespace boost::asio;
using namespace std::literals;
awaitable< void >
entry()
{
    auto exec = co_await this_coro::executor;
    std::cout << "starting entry" << std::endl;
    steady_timer timer(exec);
    auto         start_point = std::chrono::system_clock::now();
    auto         count       = 0;
    try
    {
        stream_file file(exec, "test.txt", stream_file::create | stream_file::write_only);
        std::string data(64, 'T');
        data[data.size() - 1] = '\n';
        // while (true)
        {
            while (count < 1000000)
            {
                try
                {
                    co_await async_write(file, buffer(data, data.size()), use_awaitable);
                    // [](boost::system::error_code ec, std::size_t s)
                    //{ std::cout << s << " " << ec.message() << std::endl; });
                }
                catch (std::exception exc)
                {
                    std::cout << "unknown failed " << exc.what() << std::endl;
                }
                // timer.expires_after(10ms);
                // co_await timer.async_wait(use_awaitable);
                count++;
            }
            std::cout << "done " << std::endl;
        }
    }
    catch (...)
    {
        std::cout << "failed to start" << std::endl;
    }
    std::cout
        << "Elapsed time "
        << std::chrono::duration_cast< std::chrono::seconds >(std::chrono::system_clock::now() - start_point).count()
        << std::endl;
    // timer.expires_after(60s);
    // co_await timer.async_wait(use_awaitable);
}

int
main()
{
    io_context io_ctx;
    auto       exc = io_ctx.get_executor();
    co_spawn(exc, entry(), detached);
    io_ctx.run();
}