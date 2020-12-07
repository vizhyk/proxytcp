#include <gtest/gtest.h>
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


TEST(ByteStreamTests, InsertUint8tArray)
{
    uint8_t tmp[] = {0x05,0x00,0x03};

    Proxy::ByteStream expectedStream(tmp,sizeof(tmp));
    Proxy::ByteStream tmpBuffer;

    tmpBuffer.Insert(tmp,sizeof(tmp));

    EXPECT_TRUE(tmpBuffer == expectedStream  );

    expectedStream.Insert(static_cast<uint8_t>(0x05));

    EXPECT_FALSE(tmpBuffer == expectedStream );
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

TEST(ByteStreamTests, ClearStream)
{
    Proxy::ByteStream stream;

    stream << static_cast<uint8_t>(0x05);
    stream << static_cast<uint8_t>(0x05);
    stream << static_cast<uint8_t>(0x05);
    stream << static_cast<uint8_t>(0x05);

    EXPECT_TRUE(stream.GetUsedBytes() == 4);

    stream.Clear();

    EXPECT_TRUE(stream.GetUsedBytes() == 0);
}

TEST(ByteStreamTests, UsedBytesCheckAfterResize)
{
    Proxy::ByteStream stream;

    stream << static_cast<uint8_t>(0x05);
    stream << static_cast<uint8_t>(0x05);
    stream << static_cast<uint8_t>(0x05);
    stream << static_cast<uint8_t>(0x05);

    EXPECT_TRUE(stream.GetUsedBytes() == 4);

    stream.Resize(1024);

    EXPECT_TRUE(stream.GetUsedBytes() == 4);
}

TEST(ByteStreamTests, EmpyStreamCheck)
{
    Proxy::ByteStream stream;

    EXPECT_TRUE(stream.IsEmpty());

    stream << static_cast<uint8_t>(0x05);
    EXPECT_FALSE(stream.IsEmpty());
}
