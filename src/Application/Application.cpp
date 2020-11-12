#include "Application.hpp"

namespace Proxy
{

    Application::Application(ExecutionModes::ExecutionModeType mode, const ConnectionInfo& info) noexcept
        : info_{info}, threadPool_{}
    {
        switch (mode)
        {
            case ExecutionModes::ExecutionModeType::SOCKS5:
                executionMode_ = new ExecutionModes::SOCKS5Mode;
                break;
            default:
                executionMode_ = nullptr;
                std::cout << "Incorrect execution mode option.\n";
                exit(-1);
        }
    }
} //namespace Proxy