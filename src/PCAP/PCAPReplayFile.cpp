#include "PCAPReplayFile.hpp"

#include <iterator>
#include <fstream>
#include "Utilities/Constants.hpp"

namespace Proxy::PCAP
{
    Status PCAPReplayFile::ReadAllDataFromFile()
    {
        Status status;
        //TODO: add status processing
        m_pcapfile.unsetf(std::ios::skipws);

        m_pcapFileBuffer.Insert(m_pcapfile);

        return status;
    }

    ByteStream PCAPReplayFile::RetrieveNextPacket()
    {
        ByteStream packet;

        /*
         *  auto pcapPacketSize = PCAPParser::GetPacketSize();
         *  offset += HeaderSize::PCAP_PACKET_HEADER
         *  offset += ethheader
         *
         *  srcIP = getSourceIPAddress()
         *  dstIP = ..../
         *
         *  offset += HeaderSize::IP
         *  offset += HeaderSize::TCP
         *
         *  packetPayloadSize = pcapPacketSize - HeaderSize::PCAP - HeaderSize::ETH - HeaderSize::IP - HeaderSize::TCP;
         *  packet.Insert(m_pcapFileBuffer.GetBuffer() + m_pcapFileBufferOffset, packetPayloadSize);
         */





        return packet;
    }

    PCAPReplayFile::PCAPReplayFile(const std::string& filename) noexcept
        : m_pcapfile(filename), m_pcapFileBufferOffset(HeaderSize::PCAP_GLOBAL_HEADER)
    {

    }
}


