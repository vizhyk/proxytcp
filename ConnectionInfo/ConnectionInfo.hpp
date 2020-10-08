#ifndef PROXYTCP_CONNECTIONINFO_HPP
#define PROXYTCP_CONNECTIONINFO_HPP

#include <cstdint>
#include <string>

class ConnectionInfo
{
public:
    uint32_t GetDestinationPort() const noexcept { return destinationPort; }
    uint32_t GetListeningPort() const noexcept { return listeningPort; }
    std::string GetHostName() const noexcept { return hostName; }
    std::string GetBannedHostName() const noexcept { return bannedHostName; }
private:
    uint32_t destinationPort;
    uint32_t listeningPort;
    std::string hostName;
    std::string bannedHostName;
};


#endif //PROXYTCP_CONNECTIONINFO_HPP
