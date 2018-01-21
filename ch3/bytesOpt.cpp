#include <strings.h>
#include <utility>
#include <cstring>
#include "../util/util.h"

namespace mew
{
    inline void bzero(void *dest, std::size_t nbytes)
    {
        ::bzero(dest, nbytes);
    }

    inline void bcopy(void *dest, const void *src, std::size_t nbytes)
    {
        ::bcopy(src, dest, nbytes);
    }

    inline int bcmp(const void *ptr1, const void *ptr2, std::size_t nbytes)
    {
        return ::bcmp(ptr1, ptr2, nbytes);
    }
}

int main()
{
    int x = 0;
    const int data = 0x12345678;
    std::memset(&x, 0x12345678, sizeof(x)); // 0x78787878
    std::memcpy(&x, &data, sizeof(x));
    util::outputBytesHex(reinterpret_cast<char *>(&x), sizeof(x), std::cout);
}