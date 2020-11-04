#ifndef PROXYTCP_APPLICATION_HPP
#define PROXYTCP_APPLICATION_HPP

#include "src/ExecutionModes/BanMode.hpp"
#include "src/ExecutionModes/TrackingMode.hpp"
#include "src/ExecutionModes/ExecutionMode.hpp"
#include "src/ExecutionModes/ForwardingMode.hpp"
#include "src/ExecutionModes/SOCKS5Mode.hpp"
#include "src/ConnectionInfo/ConnectionInfo.hpp"
#include "src/ThreadManager/ThreadPool.hpp"
#include "src/Utilities/NonCopyable.hpp"
#include <cstdint>

namespace Proxy
{
    class Application : public NonCopyable
    {
    public:
        Application(ExecutionModes::ExecutionModeType, const ConnectionInfo&) noexcept;

        Application(Application&& rhs) = delete;
        Application(const Application& rhs) = delete;
//        Application& operator=(Application&& rhs) = delete;
//        Application& operator=(const Application& rhs) = delete;

        ~Application() noexcept { delete executionMode_; }

    public:
        int32_t Run() noexcept { return executionMode_ ? executionMode_->Run(info_, threadPool_) : -1; }
    private:
        ConnectionInfo info_;
        ExecutionModes::ExecutionMode* executionMode_;
        ThreadPool<std::function<void()>> threadPool_;
    };

} //namespace Proxy



#endif //PROXYTCP_APPLICATION_HPP
