#ifndef PROXYTCP_CONSTANTS_HPP
#define PROXYTCP_CONSTANTS_HPP

#include <cstdint>

namespace Proxy::HeaderSizes
{

    const uint8_t ETH_HEADER = 14;
    const uint8_t IP_HEADER  = 20;
    const uint8_t TCP_HEADER = 32;

} // namespace Proxy::HeaderSizes

namespace Proxy::Utilities::Offsets
{
    namespace TLS
    {

        const uint32_t TLS_DATA = 66; // start point of the tls data;
        const uint32_t MESSAGE_TYPE = 66;
        const uint32_t HANDSHAKE_TYPE = 71;
        const uint32_t EXTENTIONS_DATA_SIZE = 178;
        const uint32_t EXTENTIONS_DATA = 180;

    } // namespace Offsets::TLS

    namespace TCP
    {

        const uint32_t TCP_DATA = 34;

    } // namespace Offsets::TCP

    namespace IP
    {

        const uint32_t IP_DATA = 14;

    } // namespace Offsets::IP

    namespace ETH
    {

    }// namespace Offsets::ETH

    namespace SOCKS5
    {
        namespace Handshake
        {
            namespace Client
            {
                const uint8_t IPv4_ADDRESS = 4;
                const uint8_t IPv6_ADDRESS = 16;
                const uint8_t DOMAIN_NAME = 5;
                const uint8_t DOMAIN_NAME_SIZE = 4;
            }  // namespace Client

            namespace Server
            {

            } // namespace Server

        } // namespace Handshake

    } // namespace Offsets::SOCKS5

} // namespace Proxy::Offsets

namespace Proxy::Utilities::Ports
{

    const int32_t HTTP = 80;
    const int32_t HTTPS = 443;

} // namespace Proxy::Utilities::Ports


#endif //PROXYTCP_CONSTANTS_HPP
