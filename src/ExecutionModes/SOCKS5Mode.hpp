#ifndef PROXYTCP_SOCKS5MODE_HPP
#define PROXYTCP_SOCKS5MODE_HPP

#include "ExecutionMode.hpp"
#include "src/ConnectionManager/ConnectionManager.hpp"
#include "src/Utilities/SOCKS5.hpp"




namespace Proxy::ExecutionModes
{
    class SOCKS5Mode : public ExecutionMode
    {
    public:
        [[noreturn]] int32_t Run(const ConnectionInfo& info,  ThreadPool<std::function<void()>>& threadPool) const noexcept override;

        static bool IsClientInitiationMessage(const char* buffer, uint32_t bufferSize) noexcept;
        static int8_t GetClientAuthenticationMethod(const char* buffer, uint32_t bufferSize) noexcept;
        int8_t GetCommand(const char* buffer, uint32_t bufferSize) const noexcept;
        static int8_t GetDestinationAddressType(const char* buffer, uint32_t bufferSize) noexcept;
        static void GetDestinationAddressAndPort(const char* buffer, uint32_t bufferSize, std::string& destinationAddress, uint16_t& port) noexcept;
        static bool EstablishConnectionWithServer(const char* buffer, uint32_t bufferSize, int32_t& serverSocket, int32_t newClientSocket) noexcept;
    };

}



#endif //PROXYTCP_SOCKS5MODE_HPP
