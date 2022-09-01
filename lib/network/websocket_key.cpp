//
// Created by tammd on 8/28/22.
//

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/system.hpp>
#include <network/websocket_key.hpp>
#include <network/ws_type.hpp>

#include <string>

namespace network::websocket
{
std::string
deduce_port(std::string const &scheme, std::string port)
{
    using boost::algorithm::iequals;

    if (port.empty())
    {
        if (iequals(scheme, "ws") or iequals(scheme, "http"))
            port = "http";
        else if (iequals(scheme, "wss") or iequals(scheme, "https"))
            port = "https";
        else
            throw boost::system::system_error(boost::asio::error::invalid_argument, "can't deduce port");
    }

    return port;
}

transport_scheme
deduce_transport(std::string const &scheme, std::string const &port)
{
    using boost::algorithm::iequals;
    if (scheme.empty())
    {
        if (port.empty())
            return transport_scheme::tcp;

        if (iequals(port, "http") or iequals(port, "ws") or iequals(port, "80"))
            return transport_scheme::tcp;

        if (iequals(port, "https") or iequals(port, "wss") or iequals(port, "443"))
            return transport_scheme::tls;

        throw boost::system::system_error(boost::asio::error::invalid_argument, "cannot deduce transport");
    }
    else
    {
        if (iequals(scheme, "http") or iequals(scheme, "ws"))
            return transport_scheme::tcp;

        if (iequals(scheme, "https") or iequals(scheme, "wss"))
            return transport_scheme::tls;

        throw boost::system::system_error(boost::asio::error::invalid_argument, "invalid scheme");
    }
}

auto
set_sni(tls_layer &stream, std::string const &host) -> void
{
    if (not SSL_set_tlsext_host_name(stream.native_handle(), host.c_str()))
        throw boost::system::system_error(
            boost::beast::error_code(static_cast< int >(::ERR_get_error()), boost::asio::error::get_ssl_category()));
}

std::string
build_target(std::string const &path, std::string const &query, std::string const &fragment)
{
    std::string result;

    if (path.empty())
        result = "/";
    else
        result = path;

    if (!query.empty())
        result += "?" + query;

    if (!fragment.empty())
        result += "#" + fragment;

    return result;
}

}   // namespace network::websocket