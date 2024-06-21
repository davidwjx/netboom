#include <iomanip>

//#include "InuLogger.h"
#include "HexHelper.h"

using namespace InuCommon;

bool CHexHelper::expect(std::istream &stream, const char *_c, const char *str)
{
    const char *p = _c;

    while (*p)
    {
        char c = *p++;

        if (expect(stream, c, str))
        {
            return true;
        }
    }

    return false;
}

bool CHexHelper::expect(std::istream &stream, const char _c, const char *str)
{
    char c = char(stream.get());

    if (stream.eof())
    {
        return true;
    }

    if (c != _c)
    {
        //LOG_ERROR("Malformed input : (" << std::string(str) << "): Expecting 0x" << std::hex << std::setw(2) << (unsigned int)_c << ", got 0x" << (unsigned int)c << "\n");
        //throw std::exception(str);
        return true;
    }

    return false;
}

void CHexHelper::outHexString(std::ostream &fOut, void *ptr, int size)
{
    fOut << std::hex << std::uppercase;

    for (int i = 0; i < size; i++)
    {
        fOut << std::setw(2) << std::setfill('0') << (unsigned int)(reinterpret_cast<unsigned int*>(ptr)[i]);
    }

    fOut << std::dec;
}

void CHexHelper::getHexValue(const char *str, void *ptr, int size)
{
    unsigned char*p = reinterpret_cast<unsigned char*>(ptr);

    for (int i = 0; i < size; i++)
    {
        unsigned char c0 = *str++;
        unsigned char c1 = *str++;

        unsigned char data = hextoint(c0) << 4 | hextoint(c1);

        *p++ = data;
    }
}

int CHexHelper::fillBuffer(const char *line_p, char *desc)
{
    int descSize = 0;

    while (*line_p)
    {
        uint8_t c0 = *line_p++;
        uint8_t c1 = *line_p++;

        uint8_t data = hextoint(c0) << 4 | hextoint(c1);

        if (desc)
        {
            *desc++ = data;
        }

        descSize++;
    }

    return descSize;
}
