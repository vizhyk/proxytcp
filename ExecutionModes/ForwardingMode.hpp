#ifndef PROXYTCP_FORWARDINGMODE_HPP
#define PROXYTCP_FORWARDINGMODE_HPP

#include "ExecutionMode.hpp"

namespace Proxy::ExecutionModes
{
    class ForwardingMode : public ExecutionMode
    {
    public:
        Utilities::Status Run(const ConnectionInfo& info) const noexcept override;

    };
} //namespace Proxy::ExecutionModes




#endif //PROXYTCP_FORWARDINGMODE_HPP
