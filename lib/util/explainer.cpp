//
// Created by tammd on 8/25/22.
//

#include <boost/asio/multiple_exceptions.hpp>
#include <util/explainer.hpp>
#include <sstream>

using namespace std::literals;
namespace util
{

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
    }
}

std::string
to_string(explainer const &e)
{
    std::ostringstream ss;
    ss << e;
    return ss.str();
}

}   // namespace util