
//
// Created by tammd on 8/27/22.
//

#include <network/event.hpp>
#include <util/format.hpp>

namespace network::websocket
{

event::event(error_code ec)
: var_(ec)
{
}

event::event(message msg)
: var_(std::move(msg))
{
}

bool
event::is_error() const noexcept
{
    return holds_alternative< error_code >(var_);
}

bool
event::is_message() const noexcept
{
    return holds_alternative< message >(var_);
}

const error_code &
event::get_error() const noexcept
{
    assert(is_error());
    return get< error_code >(var_);
}

message &
event::get_message() noexcept
{
    assert(is_message());
    return get< message >(var_);
}

message const &
event::get_message() const noexcept
{
    assert(is_message());
    return get< message >(var_);
}

std::ostream &
operator<<(std::ostream &os, event const &ev)
{
    if (ev.is_error())
        fmt::print(os, "[event [error {}]]", ev.get_error());
    else
        fmt::print(os, "[event [message {}]]", ev.get_message());
    return os;
}

}   // namespace network::websocket