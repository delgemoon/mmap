//
// Created by tammd on 8/26/22.
//

/*
 *  based on bucket toke concept
 */
#ifndef CONNECTOR_THROTTLER_HPP
#define CONNECTOR_THROTTLER_HPP

#include <boost/asio.hpp>
#include <boost/describe/enum.hpp>
#include <network/rate_limit_key.hpp>
#include <network/sliding_window_log.hpp>

#include <deque>
#include <memory>

namespace network::rate_limit
{

template < typename Impl, typename Key, typename Req >
struct basic_throttler
{
    using executor_type = boost::asio::any_io_executor;
    basic_throttler(executor_type exec, Key key);
    bool
                            pass(Req timestamp) const;
    std::shared_ptr< Impl > impl_;
    Key                     key_;
    executor_type           exec_;
};

using throttler = basic_throttler< sliding_window_impl, rate_limit_key, std::size_t >;

}   // namespace network::rate_limit

#endif   // CONNECTOR_THROTTLER_HPP
