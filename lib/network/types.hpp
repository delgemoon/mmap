//
// Created by tammd on 8/27/22.
//

#ifndef CONNECTOR_TYPES_HPP
#define CONNECTOR_TYPES_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <network/connect_option.hpp>
#include <network/connection_key.hpp>

namespace network::http
{

using tcp_layer = boost::beast::tcp_stream;
using tls_layer = boost::beast::ssl_stream< tcp_layer >;

using ws_websocket  = boost::beast::websocket::stream< tcp_layer >;
using wss_websocket = boost::beast::websocket::stream< tls_layer >;

using request_class  = boost::beast::http::request< boost::beast::http::string_body >;
using response_class = boost::beast::http::response< boost::beast::http::string_body >;
using response_type  = std::unique_ptr< response_class >;

}   // namespace network::http
#endif   // CONNECTOR_TYPES_HPP
