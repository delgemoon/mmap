//
// Created by tammd on 8/27/22.
//

#ifndef CONNECTOR_FORMAT_HPP
#define CONNECTOR_FORMAT_HPP

#endif   // CONNECTOR_FORMAT_HPP

#include <boost/optional/optional_io.hpp>
#include <boost/system/error_code.hpp>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

namespace fmt
{

template <>
struct formatter< boost::system::error_code >
{
    template < typename FormatParseContext >
    constexpr auto
    parse(FormatParseContext &ctx)
    {
        return ctx.begin();
    }
    template < typename FormatContext >
    auto
    format(const boost::system::error_code &p, FormatContext &ctx)
    {
        return format_to(ctx.out(), "{}", p.message());
    }
};

template < typename T >
struct formatter< std::optional< T > > : formatter< T >
{
    template < typename FormatParseContext >
    constexpr auto
    parse(FormatParseContext &ctx)
    {
        return ctx.begin();
    }
    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    template < typename FormatContext >
    auto
    format(const std::optional< T > &p, FormatContext &ctx)
    {
        if (p)
            return formatter< T >::format(*p, ctx);
        else
            return format_to(ctx.out(), "<null>");
    }
};

template <>
struct formatter< boost::optional< std::int64_t > >
{
    template < typename FormatParseContext >
    constexpr auto
    parse(FormatParseContext &ctx)
    {
        return ctx.begin();
    }

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    template < typename FormatContext >
    auto
    format(const std::optional< std::int64_t > &p, FormatContext &ctx)
    {
        if (p)
            return formatter::format(*p, ctx);
        else
            return format_to(ctx.out(), "<null>");
    }
};

}   // namespace fmt