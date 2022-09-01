//
// Created by tammd on 8/28/22.
//

#ifndef CONNECTOR_WS_TYPE_HPP
#define CONNECTOR_WS_TYPE_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <network/connect_option.hpp>
#include <network/connection_key.hpp>

namespace network::websocket
{

using tcp_layer = boost::beast::tcp_stream;
using tls_layer = boost::beast::ssl_stream< tcp_layer >;

using ws_websocket  = boost::beast::websocket::stream< tcp_layer >;
using wss_websocket = boost::beast::websocket::stream< tls_layer >;

}   // namespace network::websocket

#endif   // CONNECTOR_WS_TYPE_HPP
