//
// Created by tammd on 8/25/22.
//

#include <boost/asio/multiple_exceptions.hpp>
#include <boost/exception/all.hpp>
#include <fmt/ostream.h>
#include <util/explainer.hpp>

#include <exception>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <stdexcept>

using namespace std::literals;
using boost::get_error_info;
namespace util
{

void
explainer::print(std::ostream &os, std::string_view s, std::size_t) const
{
    fmt::print(os, "[exception {}]", s);
}

explainer::explainer(std::exception_ptr ep)
: ep(std::move(ep))
{
}

void
explainer::operator()(std::ostream &s) const
{
    try
    {
        if (ep)
            std::rethrow_exception(ep);
        s << "[no exception]"sv;
    }
    catch (boost::asio::multiple_exceptions &ex)
    {
        s << "[multi " << explainer(ex.first_exception()) << "]";
    }
    catch (boost::system::system_error &e)
    {
        print(s, e);
    }
    catch (std::exception &e)
    {
        print(s, e);
    }
    catch (...)
    {
        print(s, "generic error");
    }
}

std::ostream &
operator<<(std::ostream &s, explainer const &ex)
{
    ex(s);
    return s;
}

auto
explain(std::exception_ptr ep) -> explainer
{
    return explainer { ep };
}

auto
explain(const std::error_code &code) -> std::string
{
    return fmt::format("[message {}] [value {}] [category {}]",
                       std::quoted(code.message()),
                       code.value(),
                       std::quoted(code.category().name()));
}

std::string
to_string(explainer const &e)
{
    std::ostringstream ss;
    ss << e;
    return ss.str();
}

}   // namespace util
