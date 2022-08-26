//
// Created by tammd on 8/26/22.
//


#include <network/connection_key.hpp>
#include <regex>
#include <boost/asio/error.hpp>
#include <boost/algorithm/string.hpp>

namespace network::http
{

connection_key connection_key::parse_url(std::string const& url)
{
        static auto url_regex =
        std::regex(R"regex((http|https)://([^/ :]+):?([^/ ]*)((/?[^ #?]*)\x3f?([^ #]*)#?([^ ]*)))regex",
                   std::regex_constants::icase | std::regex_constants::optimize);
    std::smatch match;
    if (not std::regex_match(url, match, url_regex))
        throw boost::system::system_error(boost::asio::error::invalid_argument, "invalid url");

    auto &protocol = match[1];
    auto &host     = match[2];
    auto &port_ind = match[3];
    auto &target   = match[4];
    /*
    auto &path     = match[5];
    auto &query    = match[6];
    auto &fragment = match[7];
    */
    return connection_key {  .hostname_ = host.str(),
                             .service_  = deduce_port(protocol, port_ind),
                             .scheme_   = deduce_scheme(protocol, port_ind),
                             .target_ = target };
}

std::string
deduce_port(std::string const &scheme, std::string port)
{
    using boost::algorithm::iequals;

    if (port.empty())
    {
        if (iequals(scheme, "http"))
            return "http";
        else if (iequals(scheme, "https"))
            return "https";
        else
            throw boost::system::system_error(boost::asio::error::invalid_argument, "can't deduce port");
    }

    return port;
}

transport_scheme
deduce_scheme(std::string const &scheme, std::string const &port)
{
    using boost::algorithm::iequals;
    if (scheme.empty())
    {
        if (port.empty())
            return transport_scheme::tcp;

        if (iequals(port, "http") or iequals(port, "80"))
            return transport_scheme::tcp;

        if (iequals(port, "https") or iequals(port, "443"))
            return transport_scheme::tls;

        throw boost::system::system_error(boost::asio::error::invalid_argument, "cannot deduce transport");
    }
    else
    {
        if (iequals(scheme, "http"))
            return transport_scheme::tcp;

        if (iequals(scheme, "https"))
            return transport_scheme::tls;
        throw boost::system::system_error(boost::asio::error::invalid_argument, "invalid scheme");
    }
}

}
