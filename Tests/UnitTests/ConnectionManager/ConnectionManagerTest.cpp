#include <gtest/gtest.h>
#include "ConnectionManager/SocketConnectionManager.hpp"
#include "Status.hpp"
namespace Proxy
{

    TEST(ConnectionManagerTest, BadEpollCTL)
    {
        Status status;
        int32_t epollfd = -1;
        int32_t sockfd = -1;
        uint32_t eventFlags = 1;

        status = SocketConnectionManager::EpollAdd(epollfd, sockfd, eventFlags);
        EXPECT_EQ( status, Status::Error::BadEpollCTL);

        status = SocketConnectionManager::EpollAdd(epollfd, sockfd, eventFlags, nullptr);
        EXPECT_EQ( status, Status::Error::BadEpollCTL);
    }

    TEST(ConnectionManagerTest, BadEpollWait)
    {
        Status status;
        int32_t epollfd = 3;
        epoll_event fakeEvents[3] = {};

        status = SocketConnectionManager::EpollWait(epollfd, fakeEvents, sizeof(fakeEvents), epollfd);
        EXPECT_EQ( status, Status::Error::BadEpollWait);
    }

    TEST(ConnectionManagerTest, BadMakingSocketNonblocking)
    {
        Status status;

        status = SocketConnectionManager::MakeSocketNonblocking(-1);
        EXPECT_EQ( status, Status::Error::BadMakingSocketNonblocking);
    }

    TEST(ConnectionManagerTest, BindSocketToPort)
    {
        Status status;
        int32_t epollfd = 3;

        status = SocketConnectionManager::BindSocketToPort(epollfd, 443);
        EXPECT_EQ( status, Status::Error::BadListeningSocketInitializaton);
    }

    TEST(ConnectionManagerTest, BadAcceptNewConnection)
    {
        Status status;
        SocketConnectionManager m;
        status = m.AcceptNewConnectionSocket(0, 0);
        EXPECT_EQ( status, Status::Error::BadConnectionFromListeningSocket);
    }



};
