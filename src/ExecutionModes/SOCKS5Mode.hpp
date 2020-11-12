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
        int32_t Run(const ConnectionInfo& info,  ThreadPool<std::function<void()>>& threadPool) const noexcept override;

   };

}



#endif //PROXYTCP_SOCKS5MODE_HPP
