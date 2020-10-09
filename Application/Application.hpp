#ifndef PROXYTCP_APPLICATION_HPP
#define PROXYTCP_APPLICATION_HPP

#include "ExecutionModes/BanMode.hpp"
#include "ExecutionModes/TrackingMode.hpp"
#include "ExecutionModes/ExecutionMode.hpp"
#include "ExecutionModes/ForwardingMode.hpp"
#include "ConnectionInfo/ConnectionInfo.hpp"

#include <cstdint>

namespace Proxy
{
    class Application
    {
    public:
        Application(ExecutionModes::ExecutionModeType, const ConnectionInfo&) noexcept;

        Application(Application&& rhs) = delete;
        Application(const Application& rhs) = delete;
        Application& operator=(Application&& rhs) = delete;
        Application& operator=(const Application& rhs) = delete;

        ~Application() noexcept { delete executionMode_; }

    public:
        int32_t Run() const noexcept { return executionMode_ ? executionMode_->Run(info_) : -1; }
    private:
        ConnectionInfo info_;
        ExecutionModes::ExecutionMode* executionMode_;
    };

} //namespace Proxy



#endif //PROXYTCP_APPLICATION_HPP
