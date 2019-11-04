#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "AccessStrategy.h"


int main(int argc, char *argv[]) {
   testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
//using namespace Symposium;
//RMOAccess a1, a2;
//a1.setPrivilege("u1", privilege::readOnly);
//a1.setPrivilege("u2", privilege::modify);
//std::stringstream s;
//AccessStrategy *aa1=&a1, *aa2=&a2;
//    {
//        std::ofstream f("test.txt");
//        boost::archive::text_oarchive oa(f);
//        oa << aa1;
//    }
//    {
//        std::ifstream f("test.txt");
//        boost::archive::text_iarchive ia(f);
//        ia>>aa2;
//    }
//    RMOAccess *r1= dynamic_cast<RMOAccess *>(aa1), *r2= dynamic_cast<RMOAccess *>(aa2);
//    if(*r1==*r2)
//        std::cout<<"Ok"<<std::endl;
}