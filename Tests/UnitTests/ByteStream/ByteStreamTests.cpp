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

    Proxy::ByteStream createdFromArrayBuffer(tmp, sizeof(tmp));

    EXPECT_TRUE(createdFromArrayBuffer == tmpBuffer);
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

TEST(ByteStreamTests, InsertUint32t_64rt)
{
    Proxy::ByteStream tmpBuffer;

    tmpBuffer << static_cast<uint32_t>(0x01020304);
    tmpBuffer << static_cast<uint64_t>(0x0102030405060708);

    uint8_t tmp[12] = {0x04,0x03,0x02,0x01,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01};

    EXPECT_TRUE(memcmp(tmpBuffer.GetBuffer(),tmp,12) == 0);

}

TEST(ByteStreamTests, InsertStreamLeftShift)
{
    Proxy::ByteStream tmpBuffer;
    Proxy::ByteStream tmpBuffer1;

    tmpBuffer1 << static_cast<uint8_t>(0x01);
    tmpBuffer << tmpBuffer1;

    EXPECT_TRUE(tmpBuffer == tmpBuffer1);
}

TEST(ByteStreamTests, ClearStream)
{
    Proxy::ByteStream stream;

    stream << static_cast<uint8_t>(0x05);
    stream << static_cast<uint8_t>(0x05);
    stream << static_cast<uint8_t>(0x05);
    stream << static_cast<uint8_t>(0x05);

    EXPECT_EQ(stream.GetUsedBytes(), 4);

    stream.Clear();

    EXPECT_EQ(stream.GetUsedBytes(), 0);
}

TEST(ByteStreamTests, UsedBytesCheckAfterResize)
{
    Proxy::ByteStream stream;

    stream << static_cast<uint8_t>(0x05);
    stream << static_cast<uint8_t>(0x05);
    stream << static_cast<uint8_t>(0x05);
    stream << static_cast<uint8_t>(0x05);

    EXPECT_EQ(stream.GetUsedBytes(), 4);

    stream.Resize(1024);

    EXPECT_EQ(stream.GetUsedBytes(), 4);
}

TEST(ByteStreamTests, EmpyStreamCheck)
{
    Proxy::ByteStream stream;

    EXPECT_TRUE(stream.IsEmpty());

    stream << static_cast<uint8_t>(0x05);
    EXPECT_FALSE(stream.IsEmpty());

    EXPECT_TRUE(static_cast<bool>(stream));
}

TEST(ByteStreamTests, GetCapacity)
{
    Proxy::ByteStream tmpBuffer;

    tmpBuffer << static_cast<uint32_t>(0x01020304);
    tmpBuffer << static_cast<uint64_t>(0x0102030405060708);

    EXPECT_EQ(tmpBuffer.GetSize(),512);

}

TEST(ByteStreamTests, IsFull)
{
    Proxy::ByteStream tmpBuffer;

    tmpBuffer << static_cast<uint32_t>(0x01020304);
    tmpBuffer << static_cast<uint64_t>(0x0102030405060708);

    EXPECT_FALSE(tmpBuffer.IsFull());
}

TEST(ByteStreamTests, IteratorLoop)
{
    Proxy::ByteStream tmpBuffer;

    tmpBuffer << static_cast<uint32_t>(0x01020304);
    tmpBuffer << static_cast<uint64_t>(0x0102030405060708);

    std::size_t sum = 0;

    for(auto it = tmpBuffer.Begin(); it != tmpBuffer.End(); ++it)
    {
        sum += static_cast<uint8_t >(*it);
    }

    EXPECT_EQ(sum,46);
}

TEST(ByteStreamTests, Erase)
{
    Proxy::ByteStream stream;

    stream << static_cast<uint8_t>(0x05);
    stream << static_cast<uint8_t>(0x05);
    stream << static_cast<uint8_t>(0x05);
    stream << static_cast<uint8_t>(0x05);

    stream.Erase(stream.Begin() + 2);
    EXPECT_EQ(stream.GetUsedBytes(), 3);

    stream.Erase(stream.Begin() + 3);
    EXPECT_EQ(stream.GetUsedBytes(), 2);
}


TEST(ByteStreamTests, NotEqual)
{
    Proxy::ByteStream tmpBuffer;
    Proxy::ByteStream tmpBuffer1;

    tmpBuffer1 << static_cast<uint8_t>(0x01);
    tmpBuffer1 << static_cast<uint8_t>(0x02);

    tmpBuffer << static_cast<uint8_t>(0x01);
    tmpBuffer << static_cast<uint8_t>(0x05);


    EXPECT_TRUE(tmpBuffer != tmpBuffer1);
}
