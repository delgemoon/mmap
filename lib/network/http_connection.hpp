//
// Created by tammd on 8/27/22.
//

#ifndef CONNECTOR_HTTP_CONNECTION_HPP
#define CONNECTOR_HTTP_CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <network/connect_option.hpp>
#include <network/connection_key.hpp>
#include <network/http_connection_impl.hpp>
#include <network/types.hpp>

using namespace boost::beast;
using namespace boost::asio;

namespace network::http
{

struct connection
{
    using executor_type = connection_impl::executor_type;
    connection()        = delete;
    connection(executor_type exec, ssl::context &ssl_ctx, const connection_key &key, connect_options options);

    executor_type
    get_executor();
    boost::asio::awaitable< std::tuple< error_code, response_type > >
    get(std::string const &url, boost::beast::http::fields headers = {}, request_options options = {});
    boost::asio::awaitable< std::tuple< error_code, response_type > >
    post(std::string const         &target,
         std::string                data,
         boost::beast::http::fields headers = {},
         request_options            options = {});

    boost::asio::awaitable< std::tuple< error_code, response_type > >
    call(boost::beast::http::verb   method,
         std::string const         &target,
         std::string                data    = {},
         boost::beast::http::fields headers = {},
         request_options            options = {});

  private:
    std::shared_ptr< connection_impl > impl_;
};

}   // namespace network::http

#endif   // CONNECTOR_HTTP_CONNECTION_HPP
