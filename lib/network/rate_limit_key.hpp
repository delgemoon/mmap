//
// Created by tammd on 8/26/22.
//

#ifndef CONNECTOR_RATE_LIMIT_KEY_HPP
#define CONNECTOR_RATE_LIMIT_KEY_HPP

#include <boost/asio.hpp>
#include <boost/describe/enum.hpp>
#include <boost/json.hpp>
#include <fmt/core.h>
#include <fmt/format.h>

#include <deque>
#include <memory>

namespace network::rate_limit
{

struct rate_limit_key
{
    std::chrono::milliseconds unit_;
    int                       limit;
};

}   // namespace network::rate_limit

namespace fmt
{
template <>
struct fmt::formatter< network::rate_limit::rate_limit_key >
{
    template < typename FormatParseContext >
    constexpr auto
    parse(FormatParseContext &ctx)
    {
        return ctx.begin();
    }
    template < typename FormatParseContext >
    auto
    format(const network::rate_limit::rate_limit_key &msg, FormatParseContext &ctx)
    {
        format_to(ctx.out(), "{}@{}", msg.unit_, msg.limit);
    }
};

}   // namespace fmt
#endif   // CONNECTOR_RATE_LIMIT_KEY_HPP
