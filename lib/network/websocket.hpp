//
// Created by tammd on 8/26/22.
//

#ifndef CONNECTOR_WEBSOCKET_HPP
#define CONNECTOR_WEBSOCKET_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <network/event.hpp>

#include <memory>
#include <string>

namespace network::websocket
{

struct connection_impl;

struct connection
{
    using executor_type = boost::asio::any_io_executor;

    connection() noexcept;

    connection(std::shared_ptr< connection_impl > lifetime) noexcept;

    auto
    consume() -> boost::asio::awaitable< event >;

    auto
    has_value() const -> bool
    {
        return bool(impl_);
    }

    void
    send_text(std::string text);

    void
    send_binary(std::string binary);

    void close(boost::beast::websocket::close_reason = boost::beast::websocket::close_code::going_away);

    auto
    get_executor() const -> executor_type const &;

    /// May only be called once a close error has been indicated. To do
    /// otherwise would be a race
    /// @return a refernence to the close reason
    auto
    reason() const -> boost::beast::websocket::close_reason const &;

    auto
    invalidate() -> void
    {
        impl_.reset();
    }

  protected:
    friend auto
    operator<<(std::ostream &os, connection const &conn) -> std::ostream &;

    std::shared_ptr< connection_impl > impl_;
};

}   // namespace network::websocket

#endif   // CONNECTOR_WEBSOCKET_HPP
