#ifndef PROXYTCP_APPLICATION_HPP
#define PROXYTCP_APPLICATION_HPP

#include <cstdint>
#include "src/Utilities/NonCopyable.hpp"
#include "src/ConnectionManager/ConnectionManager.hpp"
#include "ExecutionModes/ExecutionMode.hpp"
#include "ExecutionModes/DefaultExecutionMode.hpp"
#include "ExecutionModes/CapturingExecutionMode.hpp"
#include "ExecutionModeType/ExecutionModeType.hpp"

namespace Proxy
{
    class Application : public NonCopyable
    {
    public:
        explicit Application(uint8_t executionModeID, uint16_t port, const std::string& outputFilePath) noexcept;
        Status Run() noexcept;

    private:
        std::unique_ptr<ExecutionMode> m_executionMode;
        uint16_t m_port;
    };
}




#endif //PROXYTCP_APPLICATION_HPP
