#ifndef PROXYTCP_EXECUTIONMODE_HPP
#define PROXYTCP_EXECUTIONMODE_HPP

#include <cstdint>
#include "ConnectionManager/ConnectionManager.hpp"

namespace Proxy
{
    class ExecutionMode
    {
    public:
        virtual ~ExecutionMode() = default;

        virtual Status Run(uint16_t port) const noexcept = 0;

    protected:
        std::unique_ptr<ConnectionManager> m_connectionManager;
    };
}



#endif //PROXYTCP_EXECUTIONMODE_HPP
