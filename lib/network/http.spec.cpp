//
// Created by tammd on 8/25/22.
//

#include <doctest/doctest.h>
#include <network/http.hpp>

using namespace network::http;
boost::asio::awaitable< void > main_(boost::asio::io_context& io_ctx)
{
    boost::asio::ssl::context ssl_ctx { net::ssl::context::tlsv12_client };
    auto exec = io_ctx.get_executor();
    std::shared_ptr<connection> conn_ = std::make_shared<connection>(exec, ssl_ctx, connect_options{});
    co_return;
}

TEST_CASE("http::test")
{
    boost::asio::io_context io_ctx;
    boost::asio::co_spawn(io_ctx.get_executor(), main_(io_ctx), boost::asio::detached);
    io_ctx.run();
}
