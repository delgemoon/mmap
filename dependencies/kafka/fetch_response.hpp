//
// Created by tammd on 9/6/22.
//

#ifndef CONNECTOR_DEPENDENCIES_KAFKA_FETCH_RESPONSE_HPP_74BAAC7487184A90B59F06DC0D0C7B2E
#define CONNECTOR_DEPENDENCIES_KAFKA_FETCH_RESPONSE_HPP_74BAAC7487184A90B59F06DC0D0C7B2E

#include <kafka/detail/fetch_response_iterator.hpp>
#include <kafka/detail/topics_partitions.hpp>
#include <kafka/message_fwd.hpp>
#include <kafka/primitives.hpp>
#include <kafka/response.hpp>

namespace kafka
{
struct mutable_fetch_response;

// Kafka Fetch API Response Implementation
struct fetch_response : public response< fetch_response >
{
    friend class mutable_fetch_response;

    struct partition_properties
    {
        int16       error_code;
        int64       highwater_mark_offset;
        message_set messages;
    };

    using topics_partitions = detail::topics_partitions_map< detail::empty_properties, partition_properties >;

  public:
    using topic_t      = topics_partitions::topic_type;
    using partition_t  = topics_partitions::partition_type;
    using topics_t     = topics_partitions::topics_type;
    using partitions_t = topics_partitions::partitions_type;

    const topics_t &
    topics() const;

  private:
    topics_t topics_;
};

struct mutable_fetch_response : public mutable_response< fetch_response >
{
  public:
    fetch_response::topics_t &
    mutable_topics();
};
}   // namespace kafka
#endif   // CONNECTOR_DEPENDENCIES_KAFKA_FETCH_RESPONSE_HPP_74BAAC7487184A90B59F06DC0D0C7B2E
