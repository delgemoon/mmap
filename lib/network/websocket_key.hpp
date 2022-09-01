//
// Created by tammd on 8/28/22.
//

#ifndef CONNECTOR_WEBSOCKET_KEY_HPP
#define CONNECTOR_WEBSOCKET_KEY_HPP

#include <network/ws_type.hpp>
#include <util/compare.hpp>

#include <compare>
#include <string>

namespace network::websocket
{

enum class transport_scheme
{
    tcp,
    tls
};

struct ws_key
{
    std::string      hostname_;
    std::string      service_;
    transport_scheme scheme_;
    std::string      target_;
    friend auto
    operator<=>(const ws_key &, const ws_key &) = default;
};

std::string
deduce_port(std::string const &scheme, std::string port);
transport_scheme
deduce_scheme(std::string const &scheme, std::string const &port);
auto
set_sni(tls_layer &stream, std::string const &host) -> void;
std::string
build_target(std::string const &path, std::string const &query, std::string const &fragment);

}   // namespace network::websocket

#endif   // CONNECTOR_WEBSOCKET_KEY_HPP
