//
// Created by tammd on 8/24/22.
//

#include <network/http.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <util/explainer.hpp>

namespace
{
    using namespace boost::asio::experimental::awaitable_operators;
    boost::asio::awaitable< std::tuple< error_code, boost::asio::ip::tcp::resolver ::results_type > >
    resolve(std::string const &hostname, std::string const &port, std::chrono::milliseconds timeout)
    {
        error_code                            error;
        net::ip::tcp::resolver ::results_type results;

        auto cancel = (co_await net::this_coro::cancellation_state).cancelled();
        if (cancel != net::cancellation_type::none)
        {
            error = net::error::operation_aborted;
        }
        else
        {
            auto exec = co_await net::this_coro::executor;

            auto resolver = boost::asio::ip::tcp::resolver(exec);
            auto timer    = boost::asio::steady_timer(exec);
            timer.expires_after(timeout);

            try
            {
                auto which = co_await (timer.async_wait(boost::asio::use_awaitable) ||
                                       resolver.async_resolve(hostname, port, net::use_awaitable));

                switch (which.index())
                {
                case 0:
                    error = net::error::operation_aborted;
                    break;
                case 1:
                    results = std::move(std::get< 1 >(which));
                    break;
                }
            }
            catch (...)
            {

                spdlog::default_logger_raw()->critical("{}", std::current_exception());
                throw;
            }
        }
        co_return std::make_tuple(error, std::move(results));
    }
}
namespace network::http
{
    connection::connection(connection::executor_type exec, boost::asio::ssl::context& ssl_ctx, connect_options options)
    : executor_(std::move(exec)), ssl_ctx_(ssl_ctx), options_(options), stream_(exec, ssl_ctx) {}

    boost::asio::awaitable< std::tuple< error_code ,  response_type > >
    connection::get(std::string const &url, boost::beast::http::fields headers, std::unordered_map<std::string, std::chrono::milliseconds> options)
    {
        co_return std::make_tuple(error_code{}, response_type{});
    }
    boost::asio::awaitable< std::tuple< error_code, response_type > >
    connection::post(std::string const &url, std::string data, boost::beast::http::fields headers,
                     std::unordered_map<std::string, std::chrono::milliseconds> options)
    {
        co_return std::make_tuple(error_code{}, response_type{});
    }
}

