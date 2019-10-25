cmake_minimum_required(VERSION 3.14)
project(Symposium)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib/googletest-master)
include_directories(lib/googletest-master/googletest/include)
include_directories(lib/googletest-master/googlemock/include)
include_directories(lib/hash)

add_executable(Symposium ../lib/hash/sha256.h ../lib/hash/sha256.cpp main.cpp symbol.cpp symbol.h message.cpp message.h uri.cpp uri.h user.cpp user.h filesystem.cpp filesystem.h SymServer.cpp SymServer.h document.cpp document.h SymClient.cpp SymClient.h AccessStrategy.cpp AccessStrategy.h privilege.h messageData.h privilege.cpp privilege.h tests/userTest.cpp tests/FileSystemTest.cpp tests/uriTest.cpp tests/documentTest.cpp tests/messageTest.cpp tests/AccessStrategyTest.cpp tests/symbolTest.cpp tests/SymServerTest.cpp)
target_link_libraries(Symposium gtest gtest_main gmock gmock_main)