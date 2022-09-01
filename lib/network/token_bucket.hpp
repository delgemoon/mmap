//
// Created by tammd on 8/26/22.
//

#ifndef CONNECTOR_TOKEN_BUCKET_HPP
#define CONNECTOR_TOKEN_BUCKET_HPP

#include <boost/asio.hpp>
#include <boost/describe/enum.hpp>
#include <network/rate_limit_key.hpp>

#include <deque>
#include <memory>

namespace network::rate_limit
{
struct token_bucket
{
    using executor_type = boost::asio::any_io_executor;
    explicit token_bucket(executor_type exec, rate_limit_key &key);
    std::size_t                           token_left_;
    std::deque< size_t >                  bucket_;
    std::chrono::steady_clock::time_point last_refill_ = std::chrono::steady_clock::now();
    rate_limit_key                        key_;
    executor_type                         exec_;
};
}   // namespace network::rate_limit
#endif   // CONNECTOR_TOKEN_BUCKET_HPP
