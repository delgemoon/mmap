//
// Created by tammd on 9/6/22.
//

#ifndef CONNECTOR_DEPENDENCIES_KAFKA_METADATA_RESPONSE_HPP_7DDE5CAF17E54A0E9F7DF2EE74E60D16
#define CONNECTOR_DEPENDENCIES_KAFKA_METADATA_RESPONSE_HPP_7DDE5CAF17E54A0E9F7DF2EE74E60D16

#include <vector>
#include <kafka/primitives.hpp>
#include <kafka/response.hpp>
#include <kafka/detail/topics_partitions.hpp>

namespace kafka
{

struct mutable_metadata_response;

// Kafka Metadata API response
struct metadata_response :
  public response<metadata_response>
{
  friend class mutable_metadata_response;

  struct partition_properties
  {
    using replicas_vector = std::vector<int32>;
    using isr_vector =  std::vector<int32>;
    int16 error_code;
    int32 leader;
    replicas_vector replicas;
    isr_vector isr;
  };

  struct topic_properties
  {
    int16 error_code;
  };

  using topics_partitions = detail::topics_partitions_map<
    topic_properties,
    partition_properties
  >;

public:
  struct broker_s
  {
    using optional_type = std::optional<broker_s>;
    int32 node_id;
    std::string host;
    int32 port;
  };
  using broker_vector_t = std::vector<broker_s>;
  using topic_t = topics_partitions::topic_type;
  using partition_t = topics_partitions::partition_type;
  using topics_t = topics_partitions::topics_type;
  using partitions_t = topics_partitions::partitions_type;

  const broker_vector_t& brokers() const;

  const topic_t& topics() const;

  broker_s::optional_type PartitionLeader(const std::string& topic,
                                       int32 partition) const;

private:
  broker_vector_t brokers_;
  topic_t topics_;
};

struct mutable_metadata_response :
  public mutable_response<metadata_response>
{
public:
  metadata_response::broker_vector_t & mutable_brokers();

  metadata_response::topics_t & mutable_topics();
};


}

#endif   // CONNECTOR_DEPENDENCIES_KAFKA_METADATA_RESPONSE_HPP_7DDE5CAF17E54A0E9F7DF2EE74E60D16
