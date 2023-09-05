#ifndef ENHANCE_ENCODING_BASE32_HPP
#define ENHANCE_ENCODING_BASE32_HPP

#include <string>
#include "../functions.hpp"
#include "encoder.hpp"
#include "decoder.hpp"

namespace enhance::encoding {
    class base32: public encoder, public decoder {
        static inline char map[32] = {
                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                '2', '3', '4', '5', '6', '7'
        };
    public:
        base32() = default;

        std::string encode(const std::string &str) override{
            std::string res;
            for (int i = 0; i < str.length(); i+=5){
                unsigned long long n = 0;
                size_t d = str.length() - i;
                n |= (unsigned long long)str[i] << 32;
                if (d > 1) n |= (unsigned long long)str[i+1] << 24;
                if (d > 2) n |= (unsigned long long)str[i+2] << 16;
                if (d > 3) n |= (unsigned long long)str[i+3] << 8;
                if (d > 4) n |= (unsigned long long)str[i+4];
                unsigned char n1 = (n >> 35) & 0x1F;
                res += map[n1];
                unsigned char n2 = (n >> 30) & 0x1F;
                res += map[n2];
                unsigned char n3 = (n >> 25) & 0x1F;
                res += d <= 1 ? '=' : map[n3];
                unsigned char n4 = (n >> 20) & 0x1F;
                res += d <= 1 ? '=' : map[n4];
                unsigned char n5 = (n >> 15) & 0x1F;
                res += d <= 2 ? '=' : map[n5];
                unsigned char n6 = (n >> 10) & 0x1F;
                res += d <= 3 ? '=' : map[n6];
                unsigned char n7 = (n >> 5) & 0x1F;
                res += d <= 3 ? '=' : map[n7];
                unsigned char n8 = n & 0x1F;
                res += d <= 4 ? '=' : map[n8];
            }
            return res;
        }

        std::string decode(const std::string &str) override{
            std::string res;
            for (int i = 0; i < str.length(); i+=4){
                unsigned long long n = 0;
                n |= (unsigned long long)functions::index_of(map, map + 64, str[i+7], 0);
                n |= (unsigned long long)functions::index_of(map, map + 64, str[i+6], 0) << 5;
                n |= (unsigned long long)functions::index_of(map, map + 64, str[i+5], 0) << 10;
                n |= (unsigned long long)functions::index_of(map, map + 64, str[i+4], 0) << 15;
                n |= (unsigned long long)functions::index_of(map, map + 64, str[i+3], 0) << 20;
                n |= (unsigned long long)functions::index_of(map, map + 64, str[i+2], 0) << 25;
                n |= (unsigned long long)functions::index_of(map, map + 64, str[i+1], 0) << 30;
                n |= (unsigned long long)functions::index_of(map, map + 64, str[i], 0) << 35;
                unsigned char c1 = (n >> 16) & 0xFF;
                res += (char)c1;
                unsigned char c2 = (n >> 8) & 0xFF;
                if (c2 != 0) res += (char)c2;
                unsigned char c3 = n & 0xFF;
                if (c3 != 0) res += (char)c3;
            }
            return res;
        }
    };
}

#endif
