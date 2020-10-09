#ifndef PROXYTCP_CONNECTIONINFO_HPP
#define PROXYTCP_CONNECTIONINFO_HPP

#include <cstdint>
#include <string>

class ConnectionInfo
{
public:
    ConnectionInfo(uint32_t destinationPort, uint32_t listeningPort,const std::string& hostName, const std::string& bannedHostName) noexcept
        : destinationPort_{destinationPort},listeningPort_{listeningPort},hostName_{hostName}, bannedHostName_{bannedHostName} {}
public:
    uint32_t GetDestinationPort() const noexcept { return destinationPort_; }
    uint32_t GetListeningPort() const noexcept { return listeningPort_; }
    std::string GetHostName() const noexcept { return hostName_; }
    std::string GetBannedHostName() const noexcept { return bannedHostName_; }
private:
    uint32_t destinationPort_;
    uint32_t listeningPort_;
    std::string hostName_;
    std::string bannedHostName_;
};


#endif //PROXYTCP_CONNECTIONINFO_HPP
