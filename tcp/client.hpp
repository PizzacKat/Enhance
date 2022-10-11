#ifndef ENHANCE_CLIENT_HPP
#define ENHANCE_CLIENT_HPP

#include "../socket/socket.hpp"

namespace enhance::tcp {
    class client : protected enhance::socket::socket {
    public:
        client() : enhance::socket::socket(){

        }

        client(const client &client) : socket(client) {

        }

        client(const socket &sock) : socket(sock) {

        }

        using socket::set_unblocking;
        using socket::connect;
        using socket::recv;
        using socket::send;
        using socket::close;
        using socket::connected;
    };
}

#endif //ENHANCE_CLIENT_HPP
