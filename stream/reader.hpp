#ifndef ENHANCE_READER_HPP
#define ENHANCE_READER_HPP

#include <istream>

namespace enhance::stream {
    class reader {
        std::istream &stream;
    public:
        explicit reader(std::istream &stream): stream(stream){

        }

        void read(char *data, int amount){
            stream.read(data, amount);
        }

        template <typename T>
        T read(){
            T data;
            read((char *)&data, sizeof(T));
            return data;
        }
    };
}

#endif //ENHANCE_READER_HPP
