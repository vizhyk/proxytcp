#ifndef PROXYTCP_DEFAULTEXECUTIONMODE_HPP
#define PROXYTCP_DEFAULTEXECUTIONMODE_HPP

#include "ExecutionMode.hpp"

namespace Proxy
{
    class DefaultExecutionMode : public ExecutionMode
    {
    public:
        DefaultExecutionMode();
        Status Run(uint16_t port) const noexcept override;
    };
}




#endif //PROXYTCP_DEFAULTEXECUTIONMODE_HPP
