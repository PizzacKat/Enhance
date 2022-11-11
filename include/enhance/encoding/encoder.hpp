#ifndef ENHANCE_ENCODING_ENCODER_H
#define ENHANCE_ENCODING_ENCODER_H

#include <string>

namespace enhance::encoding {
    class encoder {
    protected:
        encoder() = default;
        encoder(const encoder &) = default;
        encoder &operator=(const encoder &) = default;
    public:
        virtual std::string encode(const std::string &str) = 0;
    };
}

#endif
