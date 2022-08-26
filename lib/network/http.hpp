//
// Created by tammd on 8/24/22.
//

#ifndef MMAP_HTTP_HPP
#define MMAP_HTTP_HPP

#include <boost/beast.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ssl/context.hpp>
#include <memory>
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/core.hpp>

#include <network/connect_option.hpp>

using namespace boost::beast;
namespace network::http
{
    using tcp_layer = boost::beast::tcp_stream;
    using tls_layer = boost::beast::ssl_stream< tcp_layer >;

    using request_class  = boost::beast::http::request< boost::beast::http::string_body >;
    using response_class = boost::beast::http::response< boost::beast::http::string_body >;
    using response_type  = std::unique_ptr< response_class >;
    struct connection : public std::enable_shared_from_this< connection >
    {
        using executor_type = boost::asio::io_context::executor_type;
        using io_strand             = boost::asio::strand< boost::asio::io_context::executor_type >;
        connection() = delete;
        connection(executor_type exec, boost::asio::ssl::context& ssl_ctx, connect_options options);
        ~connection() = default;
        executor_type
        get_executor()
        {
            return executor_.get_inner_executor();
        }
        boost::asio::awaitable< std::tuple< error_code ,  response_type > >
        get(std::string const &url, boost::beast::http::fields headers = {}, std::unordered_map<std::string, std::chrono::milliseconds> options = {});
        boost::asio::awaitable< std::tuple< error_code, response_type > >
        post(std::string const &target, std::string data, boost::beast::http::fields headers = {}, std::unordered_map<std::string, std::chrono::milliseconds> options = {});
    private:
        boost::asio::awaitable< std::tuple< error_code, response_type > >
        call(boost::beast::http::verb   method,
             std::string const  &target,
             std::string         data    = {},
             boost::beast::http::fields headers = {},
             std::unordered_map<std::string, std::chrono::milliseconds> options = {});
        io_strand executor_;
        flat_buffer buffer_;
        tls_layer stream_;
        boost::asio::ssl::context& ssl_ctx_;
        connect_options  options_;
    };
}

#endif //MMAP_HTTP_HPP
