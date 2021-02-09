#include "gtest/gtest.h"
#include "Connection/ClientConnection.hpp"
#include "Connection/ServerConnection.hpp"
#include "ConversationPipeline/ConversationPipeline.hpp"
#include "ConversationManager/ConversationManager.hpp"
namespace Proxy
{

    TEST(ConnectionTest, ChangeState)
    {
        ClientConnection connection(0, Connection::ConnectionState::NotConnected, nullptr);

        connection.ChangeState(Connection::ConnectionState::Connected);

        EXPECT_EQ(connection.GetState() , Connection::ConnectionState::Connected);
    }

    TEST(ConnectionTest, ConnectionEquality)
    {
        ClientConnection  connection(0, Connection::ConnectionState::NotConnected, nullptr);
        ClientConnection connection1(2, Connection::ConnectionState::NotConnected, nullptr);
        ClientConnection connection2(0, Connection::ConnectionState::NotConnected, nullptr);
        ServerConnection connection3(4, Connection::ConnectionState::NotConnected, nullptr);


        EXPECT_TRUE(connection != connection1);
        EXPECT_TRUE(connection == connection2);
        EXPECT_TRUE(connection != connection3);
    }

    TEST(ConnectionTest, GetNullptrPipeline)
    {
        ClientConnection  connection(0, Connection::ConnectionState::NotConnected, nullptr);

        auto fakePipeline = connection.Pipeline();

        EXPECT_TRUE(fakePipeline.lock() == nullptr);
    }



};