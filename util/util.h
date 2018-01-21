#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <iomanip>
#include <cstdint>

namespace util
{
    inline void outputBytesHex(const char *buf, std::size_t len, std::ostream &os)
    {
        std::ios::fmtflags f = os.flags();
        os << std::internal << std::setfill('0') << std::setw(4)
        << std::hex << std::showbase;
        for(auto idx = 0; idx < len; ++idx)
        {
            os << std::uint32_t(std::uint8_t(buf[idx])) << ' ';
        }

        os.setf(f);
    }

    inline void outputBytesDec(const char *buf, std::size_t len, std::ostream &os)
    {
        std::ios::fmtflags f = os.flags();
        for(auto idx = 0; idx < len; ++idx)
        {
            os << int(buf[idx]) << ' ';
        }
        os.setf(f);
    }
}

#endif