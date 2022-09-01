//
// Created by tammd on 8/27/22.
//

#ifndef CONNECTOR_MESSAGE_HPP
#define CONNECTOR_MESSAGE_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <fmt/core.h>
#include <fmt/ostream.h>

#include <iomanip>
#include <map>
#include <span>
#include <string_view>
#include <strstream>
#include <variant>

namespace network::websocket
{

struct message
{
    enum class frame_type
    {
        text,
        binary
    };

    auto
    buffer() -> boost::beast::flat_buffer &
    {
        assert(empty());
        return buffer_;
    }

    auto
    type() const -> frame_type
    {
        return type_;
    }
    bool
    is_text() const
    {
        return type_ == frame_type::text;
    }
    bool
    is_binary() const
    {
        return type_ == frame_type::binary;
    }
    auto
    as_text() const -> std::string_view
    {
        auto data = buffer_.data();
        return std::string_view(reinterpret_cast< const char * >(data.data()), data.size());
    }
    auto
    as_binary() const -> std::span< const unsigned char >
    {
        auto data = buffer_.data();
        return std::span(reinterpret_cast< const unsigned char * >(data.data()), data.size());
    }
    bool
    empty() const
    {
        return size() == 0;
    }
    auto
    size() const -> std::size_t
    {
        return buffer_.size();
    }

  private:
    boost::beast::flat_buffer buffer_;
    frame_type                type_ = frame_type::text;
};
}   // namespace network::websocket

template <>
struct fmt::formatter< network::websocket::message >
{
    template < typename FormatParseContext >
    constexpr auto
    parse(FormatParseContext &ctx)
    {
        return ctx.begin();
    }
    template < typename Iterator, typename Char >
    auto
    format(const network::websocket::message &msg, fmt::basic_format_context< Iterator, Char > &ctx) -> Iterator
    {
        switch (msg.type())
        {
        case network::websocket::message::frame_type::text:
        {
            std::ostringstream ostr;
            ostr << std::quoted(msg.as_text());
            return fmt::format_to(ctx.out(), "[type text] [ data {}]", ostr.str());
        }
        case network::websocket::message::frame_type::binary:
        {
            return fmt::format_to(ctx.out(), "[type binary] [data = {:02x}]", fmt::join(msg.as_binary(), ""));
        }
        default:
            return fmt::format_to(ctx.out(), "Invalid message");
        }
    }
};

#endif   // CONNECTOR_MESSAGE_HPP
