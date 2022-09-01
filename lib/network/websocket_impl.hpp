//
// Created by tammd on 8/28/22.
//

#ifndef CONNECTOR_WEBSOCKET_IMPL_HPP
#define CONNECTOR_WEBSOCKET_IMPL_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <network/event.hpp>
#include <network/message.hpp>
#include <network/ws_type.hpp>

namespace network::websocket
{

struct connection_impl : std::enable_shared_from_this< connection_impl >
{
    using executor_type = boost::asio::any_io_executor;

    using valid_events = std::tuple< boost::beast::error_code, message >;

    connection_impl(executor_type exec);

    virtual ~connection_impl() = default;

    [[nodiscard]] virtual auto
    connect(boost::asio::ssl::context &sslctx,
            std::string                url,
            boost::beast::http::fields headers,
            std::chrono::milliseconds  pingpong_timeout = std::chrono::milliseconds(5000))
        -> boost::asio::awaitable< void >;

    executor_type const &
    get_executor() const noexcept
    {
        return exec_;
    }

    boost::asio::awaitable< std::size_t >
    send(std::string text);

    boost::asio::awaitable< void >
        close(boost::beast::websocket::close_reason = boost::beast::websocket::close_code::going_away);

    /// Consume events from the websocket. Consuming the last close or error
    /// event indicates that no more events will be available and the
    /// implementation's start() coroutine will soon complete. It is an
    /// error to consume past the error_code or close_reason
    /// @return awaitable< event_variant >
    /// @throws exception if precondition is not met
    auto
    consume() -> boost::asio::awaitable< event >;

    /// Must be called from the correct strand
    /// @return const reference
    /// @note Thread Safety: not thread safe
    auto
    error() const -> boost::beast::error_code const &
    {
        return last_error_;
    }

    auto
    reason() const -> boost::beast::websocket::close_reason const &
    {
        assert(last_error_ == boost::beast::websocket::error::closed);
        return close_reason_;
    }

    boost::asio::awaitable< void >
    wait_for_event();

    boost::asio::awaitable< void >
    start();

    boost::asio::awaitable< void >
    stop();

  protected:
    auto
    queue_event(event event) -> void;

    bool
    set_last_error(boost::beast::error_code const &ec);

    bool
    set_close_reason(boost::beast::websocket::close_reason reason);

  private:
    friend std::ostream &
    operator<<(std::ostream &os, connection_impl const &impl);

  private:
    executor_type exec_;
    wss_websocket ws_;

    boost::beast::error_code              last_error_;
    boost::beast::websocket::close_reason close_reason_;
};

}   // namespace network::websocket
#endif   // CONNECTOR_WEBSOCKET_IMPL_HPP
