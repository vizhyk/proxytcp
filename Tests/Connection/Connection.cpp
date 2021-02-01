//#include "gtest/gtest.h"
//#include "Connection/Connection.hpp"
//
//namespace Proxy
//{
//
//class ConnectionTest : public ::testing::Test
//{
//
//public:
//    Connection connection(0,0,nullptr);
//};
//
//
//TEST_F(ConnectionTest, ChangeState)
//{
//    connection.ChangeState(Connection::ConnectionState::Connected);
//    EXPECT_EQ(connection.m_state , Connection::ConnectionState::Connected);
//}
//
//TEST_F(ConnectionTest, ChangeSockfd)
//{
//    int32_t sockfd = 1;
//    connection.ChangeSockfd(sockfd);
//    EXPECT_EQ(connection.m_socket , 1);
//    connection.m_socket = 0;
//}
//
//TEST_F(ConnectionTest, GetSocketfd)
//{
//
//    connection.m_socket = 1;
//    EXPECT_EQ(connection.GetSocketfd(), 1);
//    connection.m_socket = 0;
//}
//
//TEST_F(ConnectionTest, Buffer)
//{
//    ByteStream tmp_buff(5);
//    tmp_buff.Insert(static_cast<uint8_t>(0x5));
//    tmp_buff.Insert(static_cast<uint8_t>(0x0));
//    tmp_buff.Insert(static_cast<uint8_t>(0x0));
//    tmp_buff.Insert(static_cast<uint8_t>(0x0));
//    tmp_buff.Insert(static_cast<uint8_t>(0x0));
//    connection.m_buffer.Insert(tmp_buff);
//    EXPECT_EQ(sizeof(connection.Buffer()), sizeof(tmp_buff));
//}
//
//TEST_F(ConnectionTest, shared_Pipeline)
//{
//    bool flag = false;
//    if(connection.Pipeline())
//
//    {flag = true;}
//
//    EXPECT_FALSE(flag);
//}
//
//};