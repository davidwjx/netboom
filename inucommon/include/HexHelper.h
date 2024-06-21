#pragma once

#include <iostream>
#include <iomanip>

namespace  InuCommon
{
    class CHexHelper
    {
    public:

        /**
         * \brief Convert hex char to int
         *
         * \param in[in] Chartect to convert
         *
         * \return int value of given hex char
         */
        inline static int hextoint(char in)
        {
            if (!((in >= '0' && in <= '9') || (in >= 'A' && in <= 'F')))
            {
                return -1;
                //throw std::exception("Invlaid HEX character");
            }

            return  (in <= '9') ? (in - '0') : (in - ('A' - 10));
        }

        /**
         * \brief Consume inupt with expectation
         *
         * Consume input character, matching it against expected input
         * value
         *
         * \param stream[in]    Input stream
         * \param c[in]         Charcter to match
         * \param str[in]       Error string in case of mismatch
         *
         * \return true is expectation met, felase otherwize
         */
        static bool expect(std::istream& stream, const char c, const char* str);

        /**
         * \brief Consme inupt with expectation
         *
         * Consume input characters, matching against expected input
         * value
         *
         * \param stream[in]    Input stream
         * \param c[in]         Charcter sequence to match
         * \param str[in]       Error string in case of mismatch
         *
         * \return true is expectation met, felase otherwize
         */
        static bool expect(std::istream& stream, const char* c, const char* str);

        static void outHexString(std::ostream& fOut, void* ptr, int size);

        static int fillBuffer(const char* line_p, char* desc);

        static void getHexValue(const char* str, void* ptr, int size);

        template <typename T>
        static void getHexValue(std::istream& fIn, T& v)
        {
            int size{ sizeof(T) };

            unsigned char* p = reinterpret_cast<unsigned char*>(&v);

            for (int i = 0; i < size; i++)
            {
                unsigned char c0 = fIn.get();
                unsigned char c1 = fIn.get();

                unsigned char data = hextoint(c0) << 4 | hextoint(c1);

                *p++ = data;
            }
        }

        template <typename T>
        static void outHexString(std::ostream& fOut, T& v)
        {
            unsigned char* ptr = reinterpret_cast<unsigned char*>(&v);
            int size{ sizeof(T) };

            fOut << std::hex << std::uppercase;

            for (int i = 0; i < size; i++)
            {
                fOut << std::setw(2) << std::setfill('0') << (unsigned int)(reinterpret_cast<unsigned int*>(ptr)[i]);
            }

            fOut << std::dec;
        }
    };
}