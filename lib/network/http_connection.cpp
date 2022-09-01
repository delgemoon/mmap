//
// Created by tammd on 8/27/22.
//

#include <network/http_connection.hpp>
#include <network/http_connection_impl.hpp>
#include <network/types.hpp>

namespace network::http
{

connection::connection(executor_type exec, ssl::context &ssl_ctx, const connection_key &key, connect_options options)
: impl_(std::make_shared< connection_impl >(exec, ssl_ctx, key, options))
{
}

auto
connection::get_executor() -> executor_type
{
    return impl_->get_executor();
}
boost::asio::awaitable< std::tuple< error_code, response_type > >
connection::get(std::string const &url, boost::beast::http::fields headers, request_options options)
{
    auto exec = co_await net::this_coro::executor;
    if (get_executor() == exec)
    {
        co_return co_await impl_->get(url, headers, options);
    }
    else
    {
        co_return co_await net::co_spawn(get_executor(), impl_->get(url, headers, options), use_awaitable);
    }
}
boost::asio::awaitable< std::tuple< error_code, response_type > >
connection::post(std::string const         &target,
                 std::string                data,
                 boost::beast::http::fields headers,
                 request_options            options)
{
    auto exec = co_await net::this_coro::executor;
    if (get_executor() == exec)
    {
        co_return co_await impl_->post(target, data, headers, options);
    }
    else
    {
        co_return co_await net::co_spawn(get_executor(), impl_->post(target, data, headers, options), use_awaitable);
    }
}

boost::asio::awaitable< std::tuple< error_code, response_type > >
connection::call(boost::beast::http::verb   method,
                 std::string const         &target,
                 std::string                data,
                 boost::beast::http::fields headers,
                 request_options            options)
{
    auto exec = co_await net::this_coro::executor;
    if (get_executor() == exec)
    {
        co_return co_await impl_->call(method, target, data, headers, options);
    }
    else
    {
        co_return co_await net::co_spawn(
            get_executor(), impl_->call(method, target, data, headers, options), use_awaitable);
    }
}
}   // namespace network::http