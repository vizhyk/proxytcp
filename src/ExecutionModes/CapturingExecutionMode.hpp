#ifndef PROXYTCP_CAPTURINGEXECUTIONMODE_HPP
#define PROXYTCP_CAPTURINGEXECUTIONMODE_HPP

#include "ExecutionMode.hpp"

namespace Proxy
{
    class CapturingExecutionMode : public ExecutionMode
    {
    public:
        explicit CapturingExecutionMode(std::string outputFilePath);
        Status Run(uint16_t port) const noexcept override;

    private:
        std::string m_outputFilePath;
    };
}



#endif //PROXYTCP_CAPTURINGEXECUTIONMODE_HPP
