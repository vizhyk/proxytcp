//
// Created by ofati on 10/7/20.
//

#include "Application.hpp"

namespace Proxy
{

    Application::Application(ExecutionModes::ExecutionModeType mode, const ConnectionInfo& info) noexcept
        : info_{info}
    {
        switch (mode)
        {
            case ExecutionModes::ExecutionModeType::Tracking:
                executionMode_ = new ExecutionModes::TrackingMode;
                break;
            case ExecutionModes::ExecutionModeType::Forwarding:
                executionMode_ = new ExecutionModes::ForwardingMode;
                break;
            case ExecutionModes::ExecutionModeType::Ban:
                executionMode_ = new ExecutionModes::BanMode;
                break;
            default:
                executionMode_ = nullptr;
                std::cout << "Incorrect execution mode option.\n";
                exit(-1);



        }
    }
} //namespace Proxy