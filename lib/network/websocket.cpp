//
// Created by tammd on 8/26/22.
//

#include <network/websocket.hpp>

using namespace std::literals;

namespace network::websocket
{

connection::connection() noexcept
: impl_(nullptr)
{
}

connection::connection(std::shared_ptr< connection_impl > lifetime) noexcept
: impl_(std::move(lifetime))
{
    assert(has_value());
}

void
connection::send_text(std::string text)
{
    net::co_spawn(
        get_executor(),
        [impl = impl_, text = std::move(text)]() mutable -> net::awaitable< void >
        { co_await impl->send(std::move(text)); },
        net::detached);
}

auto
connection::get_executor() const -> executor_type const &
{
    assert(impl_);
    return impl_->get_executor();
}

auto
connection::reason() const -> close_reason const &
{
    return impl_->reason();
}

auto
connection::consume() -> net::awaitable< event >
{
    auto op = [impl = impl_] { return impl->consume(); };
    if (co_await net::this_coro::executor != get_executor())
        co_return co_await net::co_spawn(get_executor(), op, net::use_awaitable);
    else
        co_return co_await op();
}

void
connection::close(boost::beast::websocket::close_reason reason)
{
    net::co_spawn(
        get_executor(),
        [impl = impl_, reason = std::move(reason)]() mutable -> net::awaitable< void >
        { co_await impl->close(std::move(reason)); },
        net::detached);
}

auto
operator<<(std::ostream &os, connection const &conn) -> std::ostream &
{
    if (conn.impl_)
        return os << *conn.impl_;
    else
        return os << "[websocket::connection null]";
}

}   // namespace network::websocket