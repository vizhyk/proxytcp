#ifndef PROXYTCP_CONNECTION_HPP
#define PROXYTCP_CONNECTION_HPP
#include <cstdint>


namespace Proxy
{
    class Connection
    {
    public:
        enum class Side : uint8_t
        {
            Client = 0x00,
            Server = 0xff,
        };

        enum class State : uint8_t
        {
            Established = 0x00,
            Undefined = 0xff,

        };

    public:
        Connection() noexcept = default;
        Connection(int32_t socket, Side type) noexcept;

        ~Connection() = default;
    public:
        int32_t GetSocketfd() const noexcept { return socket; }
        State   GetState()  const noexcept { return state; }
        Side    GetSide()   const noexcept { return  type; }

    private:
        int32_t socket;
        Side type;
        State state;
    };

    bool operator==(const Connection& lhs, const Connection& rhs) noexcept;
    bool operator!=(const Connection& lhs, const Connection& rhs) noexcept;

}



#endif //PROXYTCP_CONNECTION_HPP
