//
// Created by tammd on 8/26/22.
//

#ifndef CONNECTOR_CONNECTION_KEY_HPP
#define CONNECTOR_CONNECTION_KEY_HPP

#include <util/compare.hpp>

#include <compare>
#include <string>

namespace network::http
{

enum class transport_scheme
{
    tcp,
    tls
};

struct connection_key
{
    std::string      hostname_;
    std::string      service_;
    transport_scheme scheme_;
    std::string      target_;
    static connection_key
    parse_url(std::string const &url);
    friend auto
    operator<=>(const connection_key &, const connection_key &) = default;
};

std::string
deduce_port(std::string const &scheme, std::string port);
transport_scheme
deduce_scheme(std::string const &scheme, std::string const &port);

}   // namespace network::http

#endif   // CONNECTOR_CONNECTION_KEY_HPP
