#ifndef PROXYTCP_APPLICATION_HPP
#define PROXYTCP_APPLICATION_HPP

#include <cstdint>
#include "src/Utilities/NonCopyable.hpp"
#include "src/ConnectionManager/ConnectionManager.hpp"

namespace Proxy
{
    class Application : public NonCopyable
    {
    public:
        explicit Application(uint16_t port) noexcept;
        Status Run() noexcept;
        Status InitConnectionManager(uint8_t executionModeID, const std::string& outputFilePath);
    private:
        std::unique_ptr<ConnectionManager> m_connectionManager;
        uint16_t m_port;
    };
}




#endif //PROXYTCP_APPLICATION_HPP
