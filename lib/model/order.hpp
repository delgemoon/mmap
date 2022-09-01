//
// Created by tammd on 8/28/22.
//

#ifndef CONNECTOR_ORDER_HPP
#define CONNECTOR_ORDER_HPP

#include <model/fill.hpp>

#include <optional>
#include <string>

namespace model
{

enum class order_type : std::int8_t
{
    UNKNOWN,
    SELL,
    BUY
};
enum class order_status : std::int8_t
{
    UNKNOWN,
    OPEN,
    PARTIAL_FILL,
    FILL,
    CLOSE,
    REJECT
};

struct order
{
    std::string                                          market_;
    std::string                                          symbol_;
    std::string                                          base_;
    std::string                                          quote_;
    std::chrono::time_point< std::chrono::system_clock > created_time_;
    order_type                                           side_;
    order_status                                         last_status_;
    std::chrono::time_point< std::chrono::system_clock > last_update_;
    std::optional< std::string >                         exchange_order_id_;
    std::string                                          position_;
    order_status                                         status_;
    std::vector< fill >                                  fills;
};

}   // namespace model
#endif   // CONNECTOR_ORDER_HPP
