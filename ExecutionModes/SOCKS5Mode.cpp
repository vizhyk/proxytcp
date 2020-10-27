#include "SOCKS5Mode.hpp"

namespace Proxy::ExecutionModes
{

    [[noreturn]] int32_t SOCKS5Mode::Run(const ConnectionInfo& info, ThreadPool<std::function<void()>>& threadPool) const noexcept
    {
        std::cout << "[SOSCKS5 Mode]\n";

        Utilities::Status status {};
        sockaddr_in socketData {};
        int32_t listeningSocket {};



        status = CreateSocketOnListeningPort(listeningSocket, info.GetListeningPort(), socketData);

        while (true)
        {
            int32_t newConnectionSocket = accept(listeningSocket, nullptr, nullptr);
            if(newConnectionSocket == -1)
            {
                status = Utilities::Status(Utilities::Status::Error::BadConnectionFromListeningSocket);
                PrintStatusAndTerminateProcess(status);
            }

            threadPool.AddWork([this,newConnectionSocket, info]() {

                const uint32_t MAXBUFFERSIZE = 4096;
                char buff[MAXBUFFERSIZE];
                Utilities::Status inThreadStatus {};

                int32_t recievedData {};
                while( (recievedData = recv(newConnectionSocket,buff,MAXBUFFERSIZE,0)) > 0 )
                {

//                PrintRecievedData(buff,recievedData);

                    if(IsClientInitiationMessage(buff,recievedData))
                    {
//                        PrintRecievedData(buff,recievedData);
                        char response[2];
                        response[0] = 0x05;
                        response[1] = GetClientAuthenticationMethod(buff,recievedData);
                        if(response[1] == -1)
                        {
                            // return error
                        }

                        auto sendedData = send(newConnectionSocket,response,sizeof(response),0);
                        if(sendedData == -1)
                        {
                            std::cout << "error sending\n";
                        }
//                        PrintRecievedData(response,sendedData);

                        recievedData = recv(newConnectionSocket,buff,MAXBUFFERSIZE,0);
                        if(recievedData > 0)
                        {
                            PrintRecievedData(buff,recievedData);

                            std::string destinationAddress {};
                            uint16_t destinationPort {};

                            GetDestinationAddressAndPort(buff, recievedData, destinationAddress, destinationPort);
                            std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Destination Address/Port: " << destinationAddress << "/" << destinationPort << ".]\n";

                            int32_t socketForForwarding {};
                            int8_t addressType = GetDestinationAddressType(buff,recievedData);

                            if(addressType == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::AddressType::IPv6) ||
                               addressType == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::AddressType::IPv4))
                            {
                                inThreadStatus = CreateForwardingSocketByIP(socketForForwarding, destinationPort, destinationAddress);
                                if(inThreadStatus.Failed()) { PrintStatus(inThreadStatus); continue; }
                            }

                            if(addressType == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::AddressType::DomainName))
                            {
                                inThreadStatus = CreateForwardingSocketByHostname(socketForForwarding, destinationPort, destinationAddress);
                                if(inThreadStatus.Failed()) { PrintStatus(inThreadStatus); continue; }
                            }

                            std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Proxy->Server: Connection is successful.]\n";

                            ////////////////////// TO BE REFACTORED //////////////////////
                            char* newReply = new char[5 + destinationAddress.size() + 2 + 1];
                            newReply[0] = 0x05;
                            newReply[1] = 0x00;
                            newReply[2] = 0x00;
                            newReply[3] = addressType;

                            int8_t destinationAddressSize = destinationAddress.size();

                            memcpy(newReply + 4, &destinationAddressSize, sizeof(addressType));
                            memcpy(newReply+5,destinationAddress.c_str(),destinationAddress.size());

                            auto htonsPort = htons(destinationPort);

                            memcpy(newReply+5+destinationAddress.size(),&htonsPort,2);
                            auto replyMessageSize = 5 + destinationAddress.size() + 2;
                            ////////////////////// end //////////////////////

                            auto reply = send(newConnectionSocket, newReply, replyMessageSize, 0);
                            PrintRecievedData(newReply, replyMessageSize);
                            delete[] newReply;
                            std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Proxy->Client: " << reply << "bytes]\n";


                            char dataExchangeBuffer[MAXBUFFERSIZE];
                            int32_t clientToProxy {};
                            while((clientToProxy = recv(newConnectionSocket, dataExchangeBuffer, MAXBUFFERSIZE, 0)) > 0)
                            {
                                //PrintRecievedData(buff,clientToProxy);
                                std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Client->Proxy: " << clientToProxy << "bytes]\n";

                                int32_t proxyToServer = send(socketForForwarding, dataExchangeBuffer, clientToProxy, MSG_NOSIGNAL);
                                std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Proxy->Server: " << proxyToServer << "bytes]\n";

                                int32_t serverToProxyBytes {};
                                while((serverToProxyBytes = recv(socketForForwarding, dataExchangeBuffer, MAXBUFFERSIZE, 0)) > 0)
                                {
                                    std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Server->Proxy: " << serverToProxyBytes << "bytes]\n";

                                    uint32_t proxyToClient = send(newConnectionSocket, dataExchangeBuffer, serverToProxyBytes, 0);
                                    std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Proxy->Client: " << proxyToClient << "bytes]\n";
                                }

                            }


                        }


                    }
                    close(newConnectionSocket);
                }
                std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[All data has been sent.]\n";
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