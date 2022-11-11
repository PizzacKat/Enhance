#ifndef ENHANCE_WRITER_HPP
#define ENHANCE_WRITER_HPP

#include <ostream>

namespace enhance::stream {
    class writer {
        std::ostream &stream;
    public:
        explicit writer(std::ostream &stream): stream(stream){

        }

        void write(char *data, int amount){
            stream.write(data, amount);
        }

        template <typename T>
        void write(T &&data){
            write((char *)&data, sizeof(T));
        }

        template <typename T>
        T write(T &data){
            write(std::forward<T>(data));
        }
    };
}

#endif //ENHANCE_WRITER_HPP
