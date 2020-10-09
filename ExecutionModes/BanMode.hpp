#ifndef PROXYTCP_BANMODE_HPP
#define PROXYTCP_BANMODE_HPP

#include "ExecutionMode.hpp"

namespace Proxy::ExecutionModes
{
    class BanMode : public ExecutionMode
    {
    public:
        int32_t Run(const ConnectionInfo& info) const noexcept override;

    };
} //namespace Proxy::ExecutionModes



#endif //PROXYTCP_BANMODE_HPP
