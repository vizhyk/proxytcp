#include "gtest/gtest.h"
#include "Connection/ClientConnection.hpp"
#include "Connection/ServerConnection.hpp"
#include "ConversationPipeline/ConversationPipeline.hpp"
#include "ConversationManager/ConversationManager.hpp"
namespace Proxy
{

    TEST(ConnectionTest, ChangeState)
    {
        ClientConnection connection(0, Connection::ConnectionSide::Server, nullptr);

        connection.ChangeState(Connection::ConnectionSide::Client);

        EXPECT_EQ(connection.GetConnectionSide() , Connection::ConnectionSide::Client);
    }

    TEST(ConnectionTest, ConnectionEquality)
    {
        ClientConnection  connection(0, Connection::ConnectionSide::Server, nullptr);
        ClientConnection connection1(2, Connection::ConnectionSide::Server, nullptr);
        ClientConnection connection2(0, Connection::ConnectionSide::Server, nullptr);
        ServerConnection connection3(4, Connection::ConnectionSide::Server, nullptr);


        EXPECT_TRUE(connection != connection1);
        EXPECT_TRUE(connection == connection2);
        EXPECT_TRUE(connection != connection3);
    }

    TEST(ConnectionTest, GetNullptrPipeline)
    {
        ClientConnection  connection(0, Connection::ConnectionSide::Server, nullptr);

        auto fakePipeline = connection.Pipeline();

        EXPECT_TRUE(fakePipeline.lock() == nullptr);
    }



};