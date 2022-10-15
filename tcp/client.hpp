#ifndef ENHANCE_TCP_CLIENT_HPP
#define ENHANCE_TCP_CLIENT_HPP

#include "../socket/socket.hpp"

namespace enhance::tcp {
    class client : protected enhance::socket::socket {
    public:
        client() : enhance::socket::socket(){}
        explicit client(const socket &sock) : socket(sock) {}

        using socket::set_unblocking;
        using socket::connect;
        using socket::recv;
        using socket::close;
        using socket::connected;

        void send(const char *data, int len) const{
            int total = 0;
            while (len > total) {
                try {
                    total += socket::send(data + total, len - total);
                }catch(enhance::socket::unblocking_exception &){}
                catch(enhance::socket::socket_exception &e){
                    throw enhance::socket::socket_exception("send_all");
                }
            }
        }
    };
}

#endif //ENHANCE_CLIENT_HPP
