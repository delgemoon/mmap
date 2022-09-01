//
// Created by tammd on 8/26/22.
//

#ifndef CONNECTOR_CONNECT_OPTION_HPP
#define CONNECTOR_CONNECT_OPTION_HPP

#include <util/compare.hpp>

#include <chrono>

namespace network::http
{

struct connect_options
{
    std::chrono::milliseconds resolve_timeout   = std::chrono::seconds(5);
    std::chrono::milliseconds connect_timeout   = std::chrono::seconds(5);
    std::chrono::milliseconds handshake_timeout = std::chrono::seconds(5);
};

struct request_options
{
    std::chrono::milliseconds write_timeout = std::chrono::seconds(5);
    std::chrono::milliseconds read_timeout  = std::chrono::seconds(10);
};
}   // namespace network::http

#endif   // CONNECTOR_CONNECT_OPTION_HPP
