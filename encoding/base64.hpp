#ifndef ENHANCE_BASE64_HPP
#define ENHANCE_BASE64_HPP

#include <string>
#include "../functions.hpp"

namespace enhance::encoding {
    class base64 {
        static inline char map[64] = {
                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
        };
    public:
        static std::string encode(const std::string &str){
            std::string res;
            for (int i = 0; i < str.length(); i+=3){
                int n = 0, d = (int)str.length() - i;
                n |= str[i] << 16;
                if (d > 1) n |= str[i+1] << 8;
                if (d > 2) n |= str[i+2];
                unsigned char n1 = (n >> 18) & 0x3F;
                res += map[n1];
                unsigned char n2 = (n >> 12) & 0x3F;
                res += map[n2];
                unsigned char n3 = (n >> 6) & 0x3F;
                res += d <= 1 ? '=' : map[n3];
                unsigned char n4 = n & 0x3F;
                res += d <= 2 ? '=' : map[n4];
            }
            return res;
        }

        static std::string decode(const std::string &str){
            std::string res;
            for (int i = 0; i < str.length(); i+=4){
                int n = 0;
                n |= functions::index_of(map, map + 64, str[i+3], 0);
                n |= functions::index_of(map, map + 64, str[i+2], 0) << 6;
                n |= functions::index_of(map, map + 64, str[i+1], 0) << 12;
                n |= functions::index_of(map, map + 64, str[i], 0) << 18;
                unsigned char c1 = (n >> 16) & 0xFF;
                res += c1;
                unsigned char c2 = (n >> 8) & 0xFF;
                if (c2 != 0) res += c2;
                unsigned char c3 = n & 0xFF;
                if (c3 != 0) res += c3;
            }
            return res;
        }
    };
}

#endif
