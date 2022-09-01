//
// Created by tammd on 8/29/22.
//

#ifndef CONNECTOR_LIB_KAFKA_IMPL_CONNECTION_CONFIGURATION_HPP_68FA677999364819BBB746F677D1DE80
#define CONNECTOR_LIB_KAFKA_IMPL_CONNECTION_CONFIGURATION_HPP_68FA677999364819BBB746F677D1DE80

#include <kafka/constants.hpp>

#include <string>

namespace kafka
{

inline connection_configuration::connection_configuration()
: message_max_bytes(constants::kDefaultMessageMaxBytes)
, socket_timeout(constants::kDefaultSocketTimeout)
, client_id(constants::DefaultClientId())
, auto_connect(constants::DefaultConnectionAutoConnect())
{
}

inline void
connection_configuration::set_broker_from_string(const std::string &str)
{
    if (str.empty())
    {
        broker_address.reset();
        return;
    }
    BrokerAddress          broker;
    std::string::size_type delimiter_position = str.find(':');
    if (delimiter_position != std::string::npos && delimiter_position > 0 && delimiter_position < str.size() - 1)
    {
        broker.hostname = str.substr(0, delimiter_position);
        broker.service  = str.substr(delimiter_position + 1);
    }
    else
    {
        broker.hostname = str.substr(0, delimiter_position);
        broker.service  = constants::DefaultKafkaService();
    }
    if (broker.hostname.empty() || broker.service.empty())
    {
        return;
    }
    broker_address = broker;
}

template < typename T >
inline void
connection_configuration::set_broker(const T &broker)
{
    set_broker(broker.host, broker.port);
}

template < typename Tx, typename Ty >
inline void
connection_configuration::set_broker(const Tx &hostname, const Ty &service)
{
    broker_addr broker;
    broker.hostname = to_string(hostname);
    broker.service  = to_string< String >(service);
    broker_address  = broker;
}

}   // namespace kafka

#endif   // CONNECTOR_LIB_KAFKA_IMPL_CONNECTION_CONFIGURATION_HPP_68FA677999364819BBB746F677D1DE80
