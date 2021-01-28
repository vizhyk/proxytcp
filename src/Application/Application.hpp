#ifndef PROXYTCP_APPLICATION_HPP
#define PROXYTCP_APPLICATION_HPP

#include <src/Utilities/NonCopyable.hpp>
#include <cstdint>
#include <src/ConnectionManager/ConnectionManager.hpp>

namespace Proxy
{
    class Application : public NonCopyable
    {
    public:
        int32_t Run() noexcept;

    private:
        ConnectionManager m_connectionManager;
    };
}




#endif //PROXYTCP_APPLICATION_HPP
