//
// Created by tammd on 8/27/22.
//

#ifndef CONNECTOR_SLIDING_WINDOW_LOG_HPP
#define CONNECTOR_SLIDING_WINDOW_LOG_HPP

#include <boost/asio.hpp>
#include <network/rate_limit_key.hpp>

#include <memory>

namespace network::rate_limit
{

struct sliding_window_impl : std::enable_shared_from_this< sliding_window_impl >
{
    using executor_type = boost::asio::any_io_executor;
    explicit sliding_window_impl(executor_type exec, rate_limit_key key);
    boost::asio::awaitable< bool >
    is_pass(size_t timestamp);

  private:
    executor_type exec_;
    double        start_window;
};

}   // namespace network::rate_limit
#endif   // CONNECTOR_SLIDING_WINDOW_LOG_HPP
