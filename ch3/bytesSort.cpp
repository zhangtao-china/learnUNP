#include <netinet/in.h>
#include <iostream>
#include <cstdint>
#include <iomanip>

namespace mew
{
    inline std::uint16_t htons(std::uint16_t hs)
    {
        std::uint16_t ns = ::htons(hs);

        std::cout << "hs = " << std::internal << std::setfill('0') << std::setw(6)
        << std::hex << std::showbase << hs
        << ", htons(hs) result is " << std::setw(6) << ns << std::endl;
        
        return ns;
    }

    inline std::uint16_t ntohs(std::uint16_t ns)
    {
        std::uint16_t hs = ::htons(hs);

        std::cout << "ns = " << std::internal << std::setfill('0') << std::setw(6)
        << std::hex << std::showbase  << ns 
        << ", ntohs(ns) result is " << std::setw(6) << hs << std::endl;

        return hs;
    }

    inline std::uint32_t htonl(std::uint32_t hl)
    {
        std::uint32_t nl = ::htonl(hl);

        std::cout << "hl = " << std::internal << std::setfill('0') << std::setw(10)
        << std::hex << std::showbase 
        << hl << ", htonl(hl) result is " << std::setw(10) << nl
        << std::endl;

        return nl;
    }

    inline std::uint32_t ntohl(std::uint32_t nl)
    {
        std::uint32_t hl = ::ntohl(nl);

        std::cout << "nl = " << std::internal << std::setfill('0') << std::setw(10) 
        << std::hex << std::showbase << nl << ", " 
        << "ntohl(nl) result is " << std::setw(10) << hl << std::endl;

        return hl;
    }
}



int main()
{
    const std::uint16_t hs = 0x0102;
    const std::uint16_t ns = mew::htons(hs);
    mew::ntohs(ns);

    const std::uint32_t hl = 0x01020304;
    const std::uint32_t nl = mew::htonl(hl);
    mew::ntohl(nl);

    return 0;
}