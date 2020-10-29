#include <csignal>
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
        int32_t clientToProxyBytes {};
        int32_t serverToProxyBytes {};
        int32_t serverSocket{};
        int32_t listeningSocket {};
        int32_t recievedData {};

        char buff[MAXBUFFERSIZE];
        char serverDataExchangeBuffer[MAXBUFFERSIZE];

        bool hostIsNotBanned {false };

        status = CreateSocketOnListeningPort(listeningSocket, info.GetListeningPort(), socketData);


        while (true)
        {
            int32_t newClientSocket = accept(listeningSocket, nullptr, nullptr);
            if(newClientSocket == -1)
            {
                status = Utilities::Status(Utilities::Status::Error::BadConnectionFromListeningSocket);
                PrintStatusAndTerminateProcess(status);
            }

            threadPool.AddWork([&]() {
                std::vector<std::thread> fuckthis {};

                bool connectionToTheServerIsEstablished { false };

                while((recievedData = recv(newClientSocket, buff, MAXBUFFERSIZE, 0)) > 0)
                {
                    if(IsClientInitiationMessage(buff,recievedData))
                    {
                        char response[2];
                        response[0] = 0x05;
                        response[1] = GetClientAuthenticationMethod(buff,recievedData);
                        if(response[1] == -1)
                        {
                            // return error
                        }

                        auto sendedData = send(newClientSocket, response, sizeof(response), 0);
                        if(sendedData == -1)
                        {
                            std::cout << "error sending\n";
                        }
    //                        PrintRecievedData(response,sendedData);

                        recievedData = recv(newClientSocket, buff, MAXBUFFERSIZE, 0);
                        if(recievedData > 0)
                        {
                                PrintRecievedData(buff,recievedData);

                            std::string destinationAddress{};
                            uint16_t destinationPort{};

                            GetDestinationAddressAndPort(buff, recievedData, destinationAddress, destinationPort);
                            std::cout << "[Thread " << std::this_thread::get_id() << "]"<< "\t\t[Destination Address/Port: " << destinationAddress << "/" << destinationPort << ".]\n";


                            int8_t addressType = GetDestinationAddressType(buff, recievedData);

                            if (addressType == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::AddressType::IPv6) ||
                                addressType == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::AddressType::IPv4))
                            {
                                status = CreateForwardingSocketByIP(serverSocket, destinationPort,destinationAddress);
                                if (status.Failed()) { PrintStatus(status); continue; }
                            }

                            if (addressType == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::AddressType::DomainName))
                            {
                                status = CreateForwardingSocketByHostname(serverSocket, destinationPort, destinationAddress);
                                if (status.Failed()) { PrintStatus(status); continue; }
                            }

                            connectionToTheServerIsEstablished = true;

                            std::cout << "[Thread " << std::this_thread::get_id() << "]"
                                      << "\t\t[Proxy->Server: Connection is successful.]\n";

                            ////////////////////// TO BE REFACTORED //////////////////////
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

                            auto reply = send(newClientSocket, newReply, replyMessageSize, 0);
                            PrintRecievedData(newReply, replyMessageSize);
                            delete[] newReply;
                            std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Proxy->Client: " << reply
                                      << "bytes]\n";
                            std::cout << "Continuing\n";
                            continue;
                        }
                    }

                    if(connectionToTheServerIsEstablished)
                    {

                        if(IsClientHelloMesasge(buff, Utilities::Offsets::TLS::TLS_DATA) && recievedData > 6)
                        {
                            const std::string hostname = GetDomainNameFromTCPPacket(buff, Utilities::Offsets::TLS::TLS_DATA);
                            std::cout << "[" << hostname << "]\n";
                            hostIsNotBanned = true;
                        }
                        else
                        {
                            hostIsNotBanned = false;
                        }

                        if(hostIsNotBanned)
                        {
                            int32_t proxyToServerBytes = send(serverSocket, buff, recievedData, MSG_NOSIGNAL);
                            std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Proxy->Server: " << proxyToServerBytes << "bytes]\n";
                            if(proxyToServerBytes == -1)
                            {
                                std::cout << "[CLient " << strerror(errno) << "]\n";
                            }

                            std::thread threadSendingDataFromServer { [&] () {

                                while ((serverToProxyBytes = recv(serverSocket, serverDataExchangeBuffer, MAXBUFFERSIZE, 0)) > 0)
                                {
                                    std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Server->Proxy: " << serverToProxyBytes << "bytes]\n";

                                    int32_t proxyToClientBytes = send(newClientSocket, serverDataExchangeBuffer, serverToProxyBytes,
                                                                      MSG_NOSIGNAL);
                                    std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Proxy->Client: " << proxyToClientBytes << "bytes]\n";
                                    if (proxyToClientBytes == -1)
                                    {
                                        std::cout << "[Server " << strerror(errno) << "]\n";
                                    }
                                }
                            }};

                            fuckthis.emplace_back(std::move(threadSendingDataFromServer));

                        }
                    }

                }



            });



        }
    }

    bool SOCKS5Mode::IsClientInitiationMessage(const char* buffer, uint32_t bufferSize) const noexcept
    {
        return (bufferSize == 3) &&
               (buffer[0] == Utilities::SOCKS5::Handshake::Version) &&
               (buffer[2] == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::AuthenticationMethod::NoAuthentication));
    }

    int8_t SOCKS5Mode::GetClientAuthenticationMethod(const char* buffer, uint32_t bufferSize) const noexcept
    {
        return (bufferSize > 2) ? buffer[2] : -1;
    }

    int8_t SOCKS5Mode::GetCommand(const char* buffer, uint32_t bufferSize) const noexcept
    {
        return buffer[1];
    }

    int8_t SOCKS5Mode::GetDestinationAddressType(const char* buffer, uint32_t bufferSize) const noexcept
    {
        return  buffer[3];
    }

    void SOCKS5Mode::GetDestinationAddressAndPort(const char* buffer, uint32_t bufferSize, std::string& destinationAddress, uint16_t& port) const noexcept
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


}