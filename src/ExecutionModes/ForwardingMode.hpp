#ifndef PROXYTCP_FORWARDINGMODE_HPP
#define PROXYTCP_FORWARDINGMODE_HPP

#include "ExecutionMode.hpp"

namespace Proxy::ExecutionModes
{

    class ForwardingMode : public ExecutionMode
    {
    public:
        int32_t Run(const ConnectionInfo& info,  ThreadPool<std::function<void()>>& threadPool) const noexcept override;

    };

} //namespace Proxy::ExecutionModes




#endif //PROXYTCP_FORWARDINGMODE_HPP
