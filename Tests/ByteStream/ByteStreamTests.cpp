#include <gtest/gtest.h>
#include <iostream>
#include <sys/socket.h>

#include "src/ByteStream/ByteStream.hpp"

TEST(ByteStreamTests, InsertDataViaLeftShiftOperator)
{
    uint8_t tmp[] = {0x05,0x00,0x03};

    Proxy::ByteStream tmpBuffer;

    tmpBuffer << static_cast<uint8_t>(0x05);
    tmpBuffer << static_cast<uint8_t>(0x00);
    tmpBuffer << static_cast<uint8_t>(0x03);

    Proxy::ByteStream heapBuffer(tmp, sizeof(tmp));

    EXPECT_TRUE(heapBuffer == tmpBuffer);
}


TEST(ByteStreamTests, InterfaceRepresentationWhileUsingRecv)
{
    while(true)
    {
        Proxy::ByteStream stream(1024);
        uint8_t tmp[1024];

        auto recievedData = recv(1337, tmp, sizeof(tmp), 0);
        if(recievedData == -1)
        {
            break;
        }

        stream.Insert(tmp, sizeof(tmp));
    }
}
