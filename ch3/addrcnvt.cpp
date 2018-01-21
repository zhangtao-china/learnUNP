#include <arpa/inet.h>
#include <strings.h>
#include <iostream>
#include <cstring>
#include <cerrno>
#include "../util/util.h"

const char *pStr1 = "192.168.112.96";
const char *pStr2 = "192.168.112.096"; 
const char *pStr3 = "192.168.112. 96"; 
const char *pStr4 = "300.168.112.096"; 
const char *pStr5 = "-1.168.112.096";
const char *pStr6 = "0.0.0.0";
const char *pStr7 = "255.255.255.255";

static void my_inet_aton(const char *pStr, ::in_addr *inp)
{
    bool res = ::inet_aton(pStr, inp);
    std::cout << "[my_inet_aton] addr string is " << pStr << ", ";
    if(res)
    {
        std::cout << "convert succeed, result struct in_addr is: ";
        util::outputBytesHex(reinterpret_cast<char *>(&inp->s_addr), sizeof(inp->s_addr), std::cout);
        std::cout << std::endl;
    }
    else
    {
        std:: cout << "convert failed." << std::endl;
    }
}

static void my_inet_addr(const char *pStr)
{
    // inet_addr deprecated
    in_addr_t addr = ::inet_addr(pStr);  
    in_addr_t none = INADDR_NONE;
    std::cout << "[my_inet_addr] addr string is " << pStr << ", ";
    if(0 != std::memcmp(&addr, &none, sizeof(addr)))
    {
        std::cout << "convert succeed, result in_addr_t is: ";
        util::outputBytesHex(reinterpret_cast<char *>(&addr), sizeof(addr), std::cout); 
        std::cout << std::endl;
    }
    else
    {
        std::cout << "convert failed." << std::endl;
    }
}

static char* my_inet_ntoa(const ::in_addr &addr)
{
    std::cout << "[my_inet_ntoa] addr is ";
    util::outputBytesHex(reinterpret_cast<const char *>(&addr), sizeof(addr), std::cout);
    std::cout << ", ";
    char *pRes = ::inet_ntoa(addr);

    std::cout << "convert result is: " << pRes << std::endl;

    return pRes;
}

static int my_inet_pton(int family, const char *pStr, void *addrPtr)
{
    int res = ::inet_pton(family, pStr, addrPtr);
    std::cout << "[my_inet_pton] presentation is " << pStr << ", ";
    if(1 == res)
    {
        std::cout << "convert succeed." << std::endl;
    }
    else if(0 == res)
    {
        std::cout << "convert failed, presentation is invalid." << std::endl;
    }
    else if(-1 == res)
    {
        std::cout << "convert failed, " << std::strerror(errno) << std::endl;
    }
    else
    {
        std::cout << "convert failed, unkonwn reason, return value is " << res << std::endl;
    }

    return res;
}

static char* my_inet_ntop(int family, const void *addrptr, char *pStr, std::size_t len)
{
    const char *pRes = ::inet_ntop(family, addrptr, pStr, len);
    std::cout << "[my_inet_ntop] family is " << family << ", ";
    if(pRes)
    {
        std::cout << "convert succeed, result is " << pRes << std::endl;
    }
    else
    {
        std::cout << "convert failed." << std::endl;
    }
}

static void test_inet_aton()
{
    in_addr addr;
    ::bzero(&addr, sizeof(addr));

    my_inet_aton(pStr1, &addr);  // succeed
    my_inet_aton(pStr2, &addr);  // failed
    my_inet_aton(pStr3, &addr);  // failed
    my_inet_aton(pStr4, &addr);  // failed
    my_inet_aton(pStr5, &addr);  // failed
    my_inet_aton(pStr6, &addr);  // succeed
    my_inet_aton(pStr7, &addr);  // succeed
}

static void test_inet_addr()
{
    my_inet_addr(pStr1);  // succeed
    my_inet_addr(pStr2);  // failed
    my_inet_addr(pStr3);  // failed
    my_inet_addr(pStr4);  // failed
    my_inet_addr(pStr5);  // failed
    my_inet_addr(pStr6);  // succeed
    my_inet_addr(pStr7);  // failed
}

static void test_inet_ntoa()
{
    const int n1 = 0x6070A8C0;
    const int n2 = 0;
    const int n3 = 0xFFFFFFFF; 
    int n4 = 0x85659984;
    ::in_addr addr1, addr2, addr3, addr4;

    ::memcpy(&addr1, &n1, sizeof(addr1));
    ::memcpy(&addr2, &n2, sizeof(addr2));
    ::memcpy(&addr3, &n3, sizeof(addr3));
    ::memcpy(&addr4, &n4, sizeof(addr4));

    my_inet_ntoa(addr1);
    my_inet_ntoa(addr2);
    my_inet_ntoa(addr3);
    my_inet_ntoa(addr4);
}

static void test_inet_pton()
{
    ::in_addr addr;
    my_inet_pton(AF_INET, pStr1, &addr);
    my_inet_pton(AF_INET, pStr2, &addr);
    my_inet_pton(AF_INET, pStr3, &addr);
    my_inet_pton(AF_INET, pStr4, &addr);
    my_inet_pton(AF_INET, pStr5, &addr);
    my_inet_pton(AF_INET, pStr6, &addr);
    my_inet_pton(AF_INET, pStr7, &addr);
}

static void test_inet_ntop()
{
    const int n1 = 0x6070A8C0;
    const int n2 = 0;
    const int n3 = 0xFFFFFFFF; 
    int n4 = 0x85659984;
    ::in_addr addr1, addr2, addr3, addr4;

    ::memcpy(&addr1, &n1, sizeof(addr1));
    ::memcpy(&addr2, &n2, sizeof(addr2));
    ::memcpy(&addr3, &n3, sizeof(addr3));
    ::memcpy(&addr4, &n4, sizeof(addr4));

    char buf[64] = {};
    my_inet_ntop(AF_INET, &addr1, buf, sizeof(buf));
    my_inet_ntop(AF_INET, &addr2, buf, sizeof(buf));
    my_inet_ntop(AF_INET, &addr3, buf, sizeof(buf));
    my_inet_ntop(AF_INET, &addr4, buf, sizeof(buf));
}

int main()
{
    std::cout << "----------------- inet_aton ------------------\n";
    test_inet_aton();
    std::cout << "----------------- inet_addr -------------------\n";
    test_inet_addr();
    std::cout << "----------------- inet_ntoa -------------------\n";
    test_inet_ntoa();
    std::cout << "----------------- inet_pton -------------------\n";
    test_inet_pton();
    std::cout << "----------------- inet_ntop -------------------\n";
    test_inet_ntop();
    return 0;
}