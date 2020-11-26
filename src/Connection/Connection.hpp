#ifndef PROXYTCP_CONNECTION_HPP
#define PROXYTCP_CONNECTION_HPP
#include <cstdint>


namespace Proxy
{
    class Connection
    {
    public:

        enum class State : uint8_t
        {
            Established = 0x00,
            Undefined = 0xff,
        };

    public:
        Connection() noexcept = default;
        Connection(int32_t socket) noexcept;
        void ChangeState(State state) noexcept;
        virtual ~Connection() = default;

    public:
        int32_t GetSocketfd() const noexcept { return m_socket; }
        State   GetState()  const noexcept { return m_state; }


    private:
        int32_t m_socket;
        State m_state;
    };

    bool operator==(const Connection& lhs, const Connection& rhs) noexcept;
    bool operator!=(const Connection& lhs, const Connection& rhs) noexcept;

}

#endif //PROXYTCP_CONNECTION_HPP
