#ifndef PROXYTCP_PCAPGENERATOR_HPP
#define PROXYTCP_PCAPGENERATOR_HPP

#include "Utilities/Constants.hpp"
#include "ByteStream/ByteStream.hpp"
#include "Utilities/PCAPData.hpp"

namespace Proxy::PCAP
{
    struct Endpoint
    {
        uint32_t ipv4;
        uint16_t port;
    };

    struct DefaultEndpoints
    {
        Endpoint server;
        Endpoint client;
    };

    class PCAPGenerator
    {
    public:
        static uint32_t GetPacketSize(const uint8_t* data, std::size_t dataSize) noexcept;

    public:
        static ByteStream GenerateEthHeader() noexcept;
        static ByteStream GeneratePCAPPacketHeader(uint32_t tcpPayloadSize) noexcept;
        static ByteStream GeneratePCAPPacketHeader(uint32_t tcpPayloadSize, uint16_t TCPHeaderSize) noexcept;
        static ByteStream GeneratePCAPGlobalHeader() noexcept;
        static ByteStream GenerateIPv4Header(uint16_t tcpPayloadSize, uint32_t sourceIPv4, uint32_t desinationIPv4) noexcept;
        static ByteStream GenerateIPv4Header(uint16_t tcpPayloadSize, uint32_t sourceIPv4, uint32_t desinationIPv4, uint16_t TCPHeaderSize) noexcept;
        static ByteStream GenerateTCPHeader(uint32_t sequenceNumber, uint32_t acknowledgmentNumber, uint32_t tcpPayloadSize, uint32_t sourcePort, uint32_t destinationPort, uint16_t flags) noexcept;
        static ByteStream GenerateTCPHeader(PCAPData& senderPCAPData, PCAPData& recipientPCAPData, uint32_t tcpPayloadSize, uint32_t sourcePort, uint32_t destinationPort, uint16_t flags) noexcept;
        static ByteStream GenerateTCPHeaderSYNACKOptions(uint32_t sequenceNumber, uint32_t acknowledgmentNumber, uint32_t sourcePort, uint32_t destinationPort, uint16_t flags) noexcept;
        static ByteStream GenerateNoTCPPayloadPacket(PCAPData& lhsPCAPData, PCAPData& rhsPCAPData, uint32_t sourceIPv4, uint16_t sourcePort, uint32_t destinationIPv4, uint16_t destinationPort, uint16_t flags) noexcept;
        static ByteStream Generate3WayTCPHandshake() noexcept;

        static DefaultEndpoints GenerateNewPipelineEndpoints() noexcept;
    private:
        static const DefaultEndpoints defaultEndpoints;
        static uint32_t pipelinesCount;
    };

}



#endif //PROXYTCP_PCAPGENERATOR_HPP
