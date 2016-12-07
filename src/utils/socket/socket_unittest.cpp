#include "socket.h"
#include "config.h"

using namespace share_me_utils;

#ifdef UNITTEST
// void func() {
//     int i = 0;
//     i = 33 - 6;
//     Socket s;
    // s.Config();
// }
TEST(socket_unittest, Test) 
{ 
    ASSERT_EQ(6, 2+4); 
}
#endif
