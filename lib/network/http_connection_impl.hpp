//
// Created by tammd on 8/24/22.
//

#ifndef MMAP_HTTP_HPP
#define MMAP_HTTP_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <network/connect_option.hpp>
#include <network/connection_key.hpp>
#include <network/types.hpp>

#include <memory>
#include <unordered_map>

using namespace boost::beast;
namespace network::http
{

struct connection_impl : public std::enable_shared_from_this< connection_impl >
{
    using executor_type = boost::asio::io_context::executor_type;
    connection_impl()   = delete;
    connection_impl(executor_type              exec,
                    boost::asio::ssl::context &ssl_ctx,
                    const connection_key      &key,
                    connect_options            options);
    ~connection_impl() = default;
    executor_type
    get_executor()
    {
        return executor_;
    }
    boost::asio::awaitable< std::tuple< error_code, response_type > >
    get(std::string const &url, boost::beast::http::fields headers = {}, request_options options = {});
    boost::asio::awaitable< std::tuple< error_code, response_type > >
    post(std::string const         &target,
         std::string                data,
         boost::beast::http::fields headers = {},
         request_options            options = {});
    net::awaitable< error_code >
    connect();

    net::awaitable< std::tuple< error_code, response_type > >
    rest_call(request_class const &request, request_options const &options = {});

    boost::asio::awaitable< std::tuple< error_code, response_type > >
    call(boost::beast::http::verb   method,
         std::string const         &target,
         std::string                data    = {},
         boost::beast::http::fields headers = {},
         request_options            options = {});

  private:
    net::awaitable< error_code >
    rest_call_impl(request_class const               &request,
                   response_class                    &response,
                   request_options const             &options,
                   std::shared_ptr< connection_impl > lifetime = nullptr);

    executor_type                executor_;
    flat_buffer                  buffer_;
    std::shared_ptr< tls_layer > stream_;
    boost::asio::ssl::context   &ssl_ctx_;
    connect_options              options_;
    connection_key               key_;
};
}   // namespace network::http

#endif   // MMAP_HTTP_HPP
