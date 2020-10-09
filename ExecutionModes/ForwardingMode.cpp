#include "ForwardingMode.hpp"

namespace Proxy::ExecutionModes
{

    int32_t ForwardingMode::Run(const ConnectionInfo& info) const noexcept
    {
        std::cout << "Run mode.\n";
    }
} //namespace Proxy::ExecutionModes