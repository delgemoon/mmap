//
// Created by tammd on 8/27/22.
//

#ifndef CONNECTOR_EVENT_HPP
#define CONNECTOR_EVENT_HPP

#include <boost/asio.hpp>
#include <boost/system.hpp>
#include <boost/variant2.hpp>
#include <network/message.hpp>

#include <variant>

using namespace boost::asio;
using namespace boost::system;

namespace network::websocket
{
struct event
{
    using var = boost::variant2::variant< error_code, message >;
    var var_;

    explicit event(error_code = error::invalid_argument);

    explicit event(message);

    bool
    is_error() const noexcept;

    bool
    is_message() const noexcept;

    error_code const &
    get_error() const noexcept;

    message const &
    get_message() const noexcept;

    message &
    get_message() noexcept;

    friend std::ostream &
    operator<<(std::ostream &os, event const &ev);
};
}   // namespace network::websocket

template <>
struct fmt::formatter< network::websocket::event >
{
    template < typename FormatParseContext >
    constexpr auto
    parse(FormatParseContext &ctx)
    {
        return ctx.begin();
    }

    template < typename Iterator, typename Char >
    auto
    format(const network::websocket::event &ev, fmt::basic_format_context< Iterator, Char > &ctx) -> Iterator
    {
        if (ev.is_error())
            return fmt::format_to< "[event [error {}]]" >(ctx.out(), ev.get_error());
        else
            return fmt::format_to< "[event [message {}]]" >(ctx.out(), ev.get_message());
    }
};

#endif   // CONNECTOR_EVENT_HPP
