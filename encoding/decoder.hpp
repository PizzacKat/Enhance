#ifndef ENHANCE_ENCODING_DECODER_H
#define ENHANCE_ENCODING_DECODER_H

#include <string>

namespace enhance::encoding {
    class decoder {
    protected:
        decoder() = default;
        decoder(const decoder &) = default;
        decoder &operator=(const decoder &) = default;
    public:
        virtual std::string decode(const std::string &str) = 0;
    };
}

#endif
