//
// Created by tammd on 8/29/22.
//

#ifndef CONNECTOR_LIB_KAFKA_ERROR_HPP_3EE96C709771406EB43138C7352EBCB7
#define CONNECTOR_LIB_KAFKA_ERROR_HPP_3EE96C709771406EB43138C7352EBCB7

#ifdef ASIO_STANDALONE
#include <boost/asio/error_code.hpp>
#else
#include <boost/system/error_code.hpp>
#endif

#include <cstdint>

namespace kafka
{

enum class client_error : std::uint8_t
{
    kErrorSuccess = 0,
    kErrorAlreadyConnected,
    kErrorNotConnected,
    kErrorNoBroker,
    kErrorInProgress,
    kErrorCompressionNotAvailable,
    kErrorCompressionFailed
};

enum class kafka_error : std::int32_t
{
    kErrorNoError                             = 0,
    kErrorUnknown                             = -1,
    kErrorOffsetOutOfRange                    = 1,
    kErrorInvalidMessage                      = 2,
    kErrorUnknownTopicOrPartition             = 3,
    kErrorInvalidMessageSize                  = 4,
    kErrorLeaderNotAvailable                  = 5,
    kErrorNotLeaderForPartition               = 6,
    kErrorRequestTimedOut                     = 7,
    kErrorBrokerNotAvailable                  = 8,
    kErrorReplicaNotAvailable                 = 9,
    kErrorMessageSizeTooLarge                 = 10,
    kErrorStaleControllerEpochCode            = 11,
    kErrorOffsetMetadataTooLargeCode          = 12,
    kErrorOffsetLoadInProgressCode            = 14,
    kErrorConsumerCoordinatorNotAvailableCode = 15,
    kErrorNotCoordinatorForConsumerCode       = 16
};

}   // namespace kafka

#endif   // CONNECTOR_LIB_KAFKA_ERROR_HPP_3EE96C709771406EB43138C7352EBCB7
