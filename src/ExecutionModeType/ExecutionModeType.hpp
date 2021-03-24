#ifndef PROXYTCP_EXECUTIONMODETYPE_HPP
#define PROXYTCP_EXECUTIONMODETYPE_HPP

#include <cstdint>

namespace Proxy::ExecutionModeType
{
    constexpr uint8_t DefaultForward = 0x0f;
    constexpr uint8_t Capture = 0xf0;
    constexpr uint8_t Replay = 0xff;

} //namespace Proxy::ExecutionModeType




#endif //PROXYTCP_EXECUTIONMODETYPE_HPP
