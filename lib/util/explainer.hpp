//
// Created by tammd on 8/25/22.
//

#ifndef CONNECTOR_EXPLAINER_HPP
#define CONNECTOR_EXPLAINER_HPP

#include <boost/system/system_error.hpp>
#include <fmt/core.h>
#include <fmt/format.h>

namespace util
{
struct explainer
{
    static void
    another_what(std::exception &e, std::string &result)
    {
        try
        {
            std::rethrow_if_nested(e);
        }
        catch (std::exception &e)
        {
            result += fmt::format("\n{}", e.what());
            another_what(e, result);
        }
        catch (...)
        {
            result += fmt::format("\nnonstandard exception");
        }
    }

    explainer(std::exception_ptr ep = std::current_exception());
    void
                       operator()(std::ostream &s) const;
    std::exception_ptr ep;
    void
    print(std::ostream &os, std::string_view s, std::size_t level = 0) const;

    template < class Exception >
    void
    print(std::ostream &os, Exception &e, std::size_t level = 0) const;

    friend std::ostream &
    operator<<(std::ostream &s, explainer const &ex);
};
auto
explain(std::exception_ptr ep) -> explainer;

auto
explain(const std::error_code &ec) -> std::string;

template < class Exception >
void
explainer::print(std::ostream &os, Exception &e, std::size_t level) const
{
    try
    {
        std::rethrow_if_nested(e);
    }
    catch (boost::system::system_error &e)
    {
        print(os, e, level + 1);
    }
    catch (std::exception &e)
    {
        print(os, e, level + 1);
    }
    catch (...)
    {
        print(os, std::string_view("unknown"), level + 1);
    }
}

std::string
to_string(explainer const &e);

}   // namespace util

template <>
struct fmt::formatter< util::explainer >
{
    template < typename FormatParseContext >
    constexpr auto
    parse(FormatParseContext &ctx)
    {
        return ctx.begin();
    }

    template < typename FormatContext >
    auto
    format(const util::explainer &exp, FormatContext &ctx)
    {
        return fmt::format_to(ctx.out(), "{}", to_string(exp));
    }
};

template <>
struct fmt::formatter< std::exception_ptr >
{
    template < typename FormatParseContext >
    constexpr auto
    parse(FormatParseContext &ctx)
    {
        return ctx.begin();
    }

    template < typename FormatContext >
    auto
    format(const std::exception_ptr &exp, FormatContext &ctx)
    {
        return format_to(ctx.out(), "{}", util::explain(exp));
    }
};

#endif   // CONNECTOR_EXPLAINER_HPP
