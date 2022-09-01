//
// Created by tammd on 8/25/22.
//

#include <boost/asio/experimental/coro.hpp>
#include <boost/asio/experimental/use_coro.hpp>
#include <boost/json.hpp>
#include <doctest/doctest.h>
#include <fmt/format.h>
#include <network/connect_option.hpp>
#include <network/connection_key.hpp>
#include <network/http_connection.hpp>
#include <spdlog/spdlog.h>
#include <util/format.hpp>

#include <memory>
using namespace boost;
struct foo
{
    std::string name;
    std::string job;
};
inline foo
tag_invoke(json::value_to_tag< foo >, json::value const &jv)
{
    foo res;
    for (const auto &v : jv.as_object())
    {
        res.name = v.value().as_string();
        res.job  = v.value().as_string();
    }
    return res;
}

inline void
tag_invoke(json::value_from_tag, json::value &jv, const foo &res)
{
    auto &obj   = jv.as_object();
    obj["job"]  = res.job;
    obj["name"] = res.name;
}

boost::asio::awaitable< foo >
generate()
{
    co_return foo {};
}
using namespace network::http;
boost::asio::awaitable< void >
main_(boost::asio::io_context &io_ctx)
{
    boost::asio::ssl::context ssl_ctx { net::ssl::context::tlsv12_client };
    auto                      exec = io_ctx.get_executor();
    connection_key            key  = connection_key::parse_url("https://reqres.in");
    connection                conn(exec, ssl_ctx, key, connect_options {});
    auto [err, resp] =
        co_await conn.get("/api/products/3", boost::beast::http::fields {}, network::http::request_options {});
    if (err)
        spdlog::critical("error {}", err);
    else
    {
        spdlog::info("no error", err);
        spdlog::info("body = {}", resp->body());
        spdlog::info("status code = {}", resp->result());
        spdlog::info("size = {}", resp->payload_size());
    }
    co_return;
}

TEST_CASE("http::test")
{
    boost::asio::io_context io_ctx;
    boost::asio::co_spawn(io_ctx.get_executor(), main_(io_ctx), boost::asio::detached);
    io_ctx.run();
}
