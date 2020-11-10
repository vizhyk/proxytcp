#include <csignal>
#include <fcntl.h>
#include "SOCKS5Mode.hpp"

namespace Proxy::ExecutionModes
{

    [[noreturn]] int32_t SOCKS5Mode::Run(const ConnectionInfo& info, ThreadPool<std::function<void()>>& threadPool) const noexcept
    {
        std::cout << "[SOSCKS5 Mode]\n";
        signal(SIGPIPE, SIG_IGN);

        Utilities::Status status {};

        sockaddr_in socketData {};

        const uint32_t MAXBUFFERSIZE = 4096;

        int32_t listeningSocket {};


        status = CreateSocketOnListeningPort(listeningSocket, info.GetListeningPort(), socketData);
        if(status.Failed()) { PrintStatusAndTerminateProcess(status);}

        std::vector<std::thread> vec;

        while (true)
        {
            int32_t newClientSocket = accept(listeningSocket, nullptr, nullptr);
            if(newClientSocket == -1)
            {
                status = Utilities::Status(Utilities::Status::Error::BadConnectionFromListeningSocket);
                PrintStatusAndTerminateProcess(status);
            }

            vec.emplace_back(std::thread { [newClientSocket, &info = std::as_const(info)]() {

                int32_t serverSocket{};

                char buff[MAXBUFFERSIZE];
                int32_t recievedData {};

                bool connectionToTheServerIsEstablished { false };
                // VITALIY
                recievedData = recv(newClientSocket, buff, MAXBUFFERSIZE ,0);

                if(IsClientInitiationMessage(buff,recievedData))
                {
                    PrintNetworkData(buff,recievedData);
                    char response[2];
                    response[0] = 0x05;
                    response[1] = GetClientAuthenticationMethod(buff, recievedData);
                    if (response[1] == -1)
                    {
                        // return error
                    }

                    auto sendedData = send(newClientSocket, response, sizeof(response), 0);
                    if (sendedData == -1)
                    {
                        std::cout << "error sending\n";
                    }
                    // 0x05 0x00
                }

                recievedData = recv(newClientSocket, buff, MAXBUFFERSIZE, 0);
                if(recievedData > 0)
                {
                    PrintNetworkData(buff,recievedData);
                    // finish SOCKS5 handshake
                    connectionToTheServerIsEstablished = EstablishConnectionWithServer(buff,recievedData,serverSocket,newClientSocket);
                }

                // VITALIY DO SYUDA
                if(connectionToTheServerIsEstablished)
                {
                    std::thread t1 { [newClientSocket,serverSocket, &info]() {
                        char tmp[MAXBUFFERSIZE];
                        int32_t rcv {};
                        while((rcv = recv(newClientSocket, tmp, MAXBUFFERSIZE, 0)) > 0)
                        {
                            std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[CLient->Proxy: " << rcv << "bytes]\n";

                            if(IsClientHelloMesasge(tmp,Utilities::Offsets::TLS::TLS_DATA))
                            {
                                const std::string hostname = GetDomainNameFromTCPPacket(tmp, Utilities::Offsets::TLS::TLS_DATA);
                                if(hostname == info.GetBannedHostName())
                                {
                                    std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Connection from " << hostname << " refused. Closing sockets\n";
//                                    shutdown(serverSocket, )

                                }
                            }

                            int32_t proxyToServerBytes = send(serverSocket, tmp, rcv, MSG_NOSIGNAL);
                            std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Proxy->Server: " << proxyToServerBytes << "bytes]\n";
                            if(proxyToServerBytes == -1)
                            {
                                std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[CLient " << strerror(errno) << "]\n";
                            }
                        }

                    }};


                    std::thread t2 { [newClientSocket,serverSocket]() {
                        int32_t serverToProxyBytes{};
                        char tmp[MAXBUFFERSIZE];
                        while((serverToProxyBytes = recv(serverSocket, tmp, MAXBUFFERSIZE, 0)) > 0)
                        {
                            std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Server->Proxy: " << serverToProxyBytes << "bytes]\n";

                            int32_t proxyToClientBytes = send(newClientSocket, tmp, serverToProxyBytes,MSG_NOSIGNAL);
                            std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Proxy->Client: " << proxyToClientBytes << "bytes]\n";
                            if (proxyToClientBytes == -1)
                            {
                                std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Send Server " << strerror(errno) << "]\n";
                            }
                        }
                    }};

                    t1.join();
                    t2.join();

                }


            }});


        }
    }

    bool SOCKS5Mode::IsClientInitiationMessage(const char* buffer, uint32_t bufferSize) noexcept
    {
        return (buffer[0] == Utilities::SOCKS5::Handshake::Version);

//               (buffer[2] == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::AuthenticationMethod::NoAuthentication));
// 0x5 0xFF
    }
// 0x5 0x1 00
    int8_t SOCKS5Mode::GetClientAuthenticationMethod(const char* buffer, uint32_t bufferSize) noexcept
    {
        return (bufferSize > 2) ? buffer[2] : -1;
    }

    int8_t SOCKS5Mode::GetCommand(const char* buffer, uint32_t bufferSize) const noexcept
    {
        return buffer[1];
    }

    int8_t SOCKS5Mode::GetDestinationAddressType(const char* buffer, uint32_t bufferSize) noexcept
    {
        return  buffer[3];
    }

    void SOCKS5Mode::GetDestinationAddressAndPort(const char* buffer, uint32_t bufferSize, std::string& destinationAddress, uint16_t& port) noexcept
    {
        using namespace Utilities;

        Status status{};

        char* tmpDestinationAddress {nullptr};
        uint8_t tmpDestinationAddressSize {};

        switch(GetDestinationAddressType(buffer,bufferSize))
        {
            case static_cast<uint8_t>(SOCKS5::Handshake::AddressType::IPv4):
            {
                tmpDestinationAddressSize = 4;
                tmpDestinationAddress = new char[tmpDestinationAddressSize + 1]; // +1 for C string null-terminator;
                memcpy(tmpDestinationAddress,buffer + Offsets::SOCKS5::Handshake::Client::IP_ADDRESS,tmpDestinationAddressSize);

                auto tmpPortOffset = Offsets::SOCKS5::Handshake::Client::IP_ADDRESS + tmpDestinationAddressSize;

                memcpy(&port,buffer + tmpPortOffset, tmpDestinationAddressSize);
                port = ntohs(port);
                break;                
            }
            case static_cast<uint8_t>(SOCKS5::Handshake::AddressType::DomainName):
            {
                memcpy(&tmpDestinationAddressSize,buffer + Offsets::SOCKS5::Handshake::Client::DOMAIN_NAME_SIZE, sizeof(tmpDestinationAddressSize));
                tmpDestinationAddress = new char[tmpDestinationAddressSize + 1]; // +1 for C string null-terminator;
                memcpy(tmpDestinationAddress,buffer + Offsets::SOCKS5::Handshake::Client::DOMAIN_NAME,tmpDestinationAddressSize);

                auto tmpPortOffset =  Offsets::SOCKS5::Handshake::Client::DOMAIN_NAME + tmpDestinationAddressSize;

                memcpy(&port,buffer + tmpPortOffset, tmpDestinationAddressSize);
                port = ntohs(port);

                break;
            }
            case static_cast<uint8_t>(SOCKS5::Handshake::AddressType::IPv6):
            {
                tmpDestinationAddressSize = 16;
                tmpDestinationAddress = new char[tmpDestinationAddressSize + 1]; // +1 for C string null-terminator;
                memcpy(tmpDestinationAddress,buffer + Offsets::SOCKS5::Handshake::Client::IP_ADDRESS,tmpDestinationAddressSize);

                auto tmpPortOffset = Offsets::SOCKS5::Handshake::Client::IP_ADDRESS + tmpDestinationAddressSize;

                memcpy(&port,buffer + tmpPortOffset, tmpDestinationAddressSize);
                port = ntohs(port);



                break;  
            }

        }
        destinationAddress = std::string(tmpDestinationAddress,tmpDestinationAddressSize);
        delete[] tmpDestinationAddress;
    }

    bool SOCKS5Mode::EstablishConnectionWithServer(const char* buffer, uint32_t bufferSize, int32_t& serverSocket, int32_t newClientSocket) noexcept
    {
        Utilities::Status status{};

        std::string destinationAddress{};
        uint16_t destinationPort{};

        GetDestinationAddressAndPort(buffer, bufferSize, destinationAddress, destinationPort);
        std::cout << "[Thread " << std::this_thread::get_id() << "]"<< "\t\t[Destination Address/Port: " << destinationAddress << "/" << destinationPort << ".]\n";

        int8_t addressType = GetDestinationAddressType(buffer, bufferSize);

        if (addressType == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::AddressType::IPv6) ||
            addressType == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::AddressType::IPv4))
        {
            status = CreateForwardingSocketByIP(serverSocket, destinationPort,destinationAddress);
            if (status.Failed()) { PrintStatus(status); return false; }
        }

        if (addressType == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::AddressType::DomainName))
        {
            status = CreateForwardingSocketByHostname(serverSocket, destinationPort, destinationAddress);
            if (status.Failed()) { PrintStatus(status); return false; }
        }


        std::cout << "[Thread " << std::this_thread::get_id() << "]"
                  << "\t\t[Proxy->Server: Connection is successful.]\n";

        ////////////////////// TO BE REFACTORED ////////////////////// Proxy response to the client
        char* newReply = new char[5 + destinationAddress.size() + 2 + 1];
        newReply[0] = 0x05;
        newReply[1] = 0x00;
        newReply[2] = 0x00;
        newReply[3] = addressType;

        int8_t destinationAddressSize = destinationAddress.size();

        memcpy(newReply + 4, &destinationAddressSize, sizeof(addressType));
        memcpy(newReply + 5, destinationAddress.c_str(), destinationAddress.size());

        auto htonsPort = htons(destinationPort);

        memcpy(newReply + 5 + destinationAddress.size(), &htonsPort, 2);
        auto replyMessageSize = 5 + destinationAddress.size() + 2;
        ////////////////////// end //////////////////////

        auto reply = send(newClientSocket, newReply, replyMessageSize, MSG_NOSIGNAL);
        PrintNetworkData(newReply, replyMessageSize);
        delete[] newReply;
        std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Proxy->Client: " << reply
                  << "bytes]\n";

        return true;
    }


}