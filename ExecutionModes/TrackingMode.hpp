#ifndef PROXYTCP_TRACKINGMODE_HPP
#define PROXYTCP_TRACKINGMODE_HPP

#include "ExecutionMode.hpp"

namespace Proxy::ExecutionModes
{
    class TrackingMode : public ExecutionMode
    {
        int32_t Run(const ConnectionInfo& info) const noexcept override;
    };
} //namespace Proxy::ExecutionModes




#endif //PROXYTCP_TRACKINGMODE_HPP
