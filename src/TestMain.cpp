#include <gtest/gtest.h>
#include "src/NetworkTrafficParser/SOCKSParser.h"

#include "src/ExecutionModes/ExecutionMode.hpp"
#include "src/Utilities/SOCKS5.hpp"

    class SOCKSParserTEST :public SOCKSParser, public::testing::Test{

    public:
        uint32_t buffersize  = 255;
        uint8_t buffersize_local = 1;
        SOCKSParser test_obj;
    };


    TEST_F(SOCKSParserTEST, IsClientInitiationMessage_true) {
        const unsigned char buf[2] ={0x00,0x00};
        const unsigned char buf1[2] ={0x00,0x02};
        const unsigned char buf2[2] ={0x00,0x03};
        const unsigned char buf3[2] ={0x00,0xFE};

        EXPECT_TRUE( test_obj.IsClientInitiationMessage(buf , buffersize));
        EXPECT_TRUE( test_obj.IsClientInitiationMessage(buf1 , buffersize));
        EXPECT_TRUE( test_obj.IsClientInitiationMessage(buf2 , buffersize));
        EXPECT_TRUE( test_obj.IsClientInitiationMessage(buf3 , buffersize));
    }


    TEST_F(SOCKSParserTEST, IsClientInitiationMessage_false) {
        const unsigned char buf[2] = {0x00, 0x01};
        const unsigned char buf1[2] = {0x00, 0xFF};

        EXPECT_FALSE( test_obj.IsClientInitiationMessage(buf , buffersize));
        EXPECT_FALSE(test_obj.IsClientInitiationMessage(buf1, buffersize));
    }


    TEST_F(SOCKSParserTEST, GetClientAuthenticationMethod)
    {
        const char buf[3] ={0x00,0x01,0x02};

        EXPECT_EQ( test_obj.GetClientAuthenticationMethod(buf , buffersize_local), -1);
        EXPECT_EQ( test_obj.GetClientAuthenticationMethod(buf , buffersize), 0x02);
    }

    TEST_F(SOCKSParserTEST, GetDestinationAddressType) {
        const char buf[3] ={0x00,0x01,0x02};

        EXPECT_EQ( test_obj.GetClientAuthenticationMethod(buf , buffersize), 0x02);
    }

