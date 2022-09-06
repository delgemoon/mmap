//
// Created by tammd on 8/30/22.
//

#ifndef CONNECTOR_LIB_KAFKA_PRODUCT_REQUEST_HPP_2384E7A997EC479184C7A5F9F3A8E6F2
#define CONNECTOR_LIB_KAFKA_PRODUCT_REQUEST_HPP_2384E7A997EC479184C7A5F9F3A8E6F2

#include <kafka/request.hpp>

namespace kafka
{

struct product_request : public request< product_request >
{
};

}   // namespace kafka

#endif   // CONNECTOR_LIB_KAFKA_PRODUCT_REQUEST_HPP_2384E7A997EC479184C7A5F9F3A8E6F2
