#include "ForwardingMode.hpp"

namespace Proxy::ExecutionModes
{

    Utilities::Status ForwardingMode::Run(const ConnectionInfo& info) const noexcept
    {
        std::cout << "Run mode.\n";
    }
} //namespace Proxy::ExecutionModes