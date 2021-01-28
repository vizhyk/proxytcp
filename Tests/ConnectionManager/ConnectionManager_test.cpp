#include <gtest/gtest.h>
#include "ConnectionManager/ConnectionManager.hpp"
#include "Status.hpp"
namespace Proxy
{
    class ConnectionManagerTest : public ::testing::Test
    {

    public:
        ConnectionManager object;
    };



TEST_F(ConnectionManagerTest, EpollAdd)
{
    int32_t epollfd = 3;
    int32_t sockfd = 4;
    uint32_t eventFlags = 1;
    EXPECT_EQ(ConnectionManager::EpollAdd(epollfd, sockfd, eventFlags), Status::Error::BadEpollCTL);
}

TEST_F(ConnectionManagerTest, BindSocketToPort)
{
    int32_t sockfd = 0;
    uint16_t port = 0;


    EXPECT_EQ(ConnectionManager::BindSocketToPort(sockfd, port),Status::Success::Success);
}

TEST_F(ConnectionManagerTest, MakeSocketNonblocking)
{
    int32_t  sockfd = 0;
    EXPECT_EQ(ConnectionManager::MakeSocketNonblocking(sockfd), Status::Success::Success);
}

TEST_F(ConnectionManagerTest, EpollCreate)
{
    int32_t epollfd = 0;
    EXPECT_EQ(ConnectionManager::EpollCreate(epollfd) , Status::Success::Success);

}

TEST_F(ConnectionManagerTest, EpollWait)
{
    int32_t epollfd = 1;
    epoll_event* epollEvents;
    uint32_t epollEventsSize = 1;
    int32_t socketsWithEvent = 1;
    EXPECT_EQ(ConnectionManager::EpollWait(epollfd, epollEvents, epollEventsSize, socketsWithEvent), Status::Error::BadEpollWait);
}

TEST_F(ConnectionManagerTest, AcceptNewConnectionSocket)
{
    int32_t listeningsocket = 1;
    int32_t epollfd = 1;
    EXPECT_EQ(object.ConnectionManager::AcceptNewConnectionSocket(listeningsocket,epollfd), Status::Error::BadConnectionFromListeningSocket);
}

};
