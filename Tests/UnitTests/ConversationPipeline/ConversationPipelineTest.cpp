#include <gtest/gtest.h>
#include <ConversationManager/ConversationManager.hpp>
#include "ConversationPipeline/ConversationPipeline.hpp"
#include "ConversationFlow/SOCKS5Flow/ClientHelloTransmission.hpp"
namespace Proxy
{

    TEST(ConversationPipelineTests, TestGetters)
    {
        ConversationManager manager;
        std::unique_ptr<ConversationFlow> newFlow = std::make_unique<SOCKS5Flow::ClientHelloTransmission>();

        auto pipeline = std::make_shared<ConversationPipeline>(0,std::move(newFlow),manager);

        pipeline->InitClientConnection(0);

        EXPECT_EQ(pipeline->GetClientSockfd(), 0);
        EXPECT_EQ(pipeline->GetServerSockfd(), -1);
        EXPECT_EQ(pipeline->GetEpollfd(), 0);
        EXPECT_TRUE(pipeline->IsClientConnectionInitialized());
        EXPECT_FALSE(pipeline->IsServerConnectionInitialized());


    }
}