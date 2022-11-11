#ifndef ENHANCE_TCP_SERVER_HPP
#define ENHANCE_TCP_SERVER_HPP

#include "../socket/socket.hpp"

namespace enhance::tcp {
    class server : protected enhance::socket::socket {
    public:
        server() : enhance::socket::socket(){
            //set_unblocking(false);
        }

        using socket::set_unblocking;
        using socket::bind;
        using socket::listen;
        using socket::accept;
        using socket::close;
        using socket::connected;
    };
}

#endif
