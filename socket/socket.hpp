#ifndef ENHANCE_SOCKET_HPP
#define ENHANCE_SOCKET_HPP

#include "base.hpp"

namespace enhance::socket {
    class socket {
    protected:
        typedef unsigned long long socket_t;

        socket(socket_t socket){
            m_socket = socket;
        }

        socket_t m_socket = -1;
    public:
        socket(const socket &socket){
            m_socket = socket.m_socket;
        }

        explicit socket(int af = AF_INET, int type = SOCK_STREAM, int proto = IPPROTO_TCP){
            m_socket = ::socket(af, type, proto);
            if (m_socket == -1) throw socket_exception("socket");
        }

        void set_unblocking(u_long blocking) const {
#ifdef _WIN32
            ioctlsocket(m_socket, FIONBIO, &blocking);
#endif
#ifdef linux
            fnctl_nonblock(m_socket);
#endif
        }

        void bind(const ip_address &addr) const{
            const sockaddr_in saddr = (const sockaddr_in)addr;
            if (::bind(m_socket, (sockaddr *)&saddr, sizeof(sockaddr_in)) < 0) throw socket_exception("bind");
        }

        void listen() const {
            if (::listen(m_socket, 3) < 0) throw socket_exception("listen");
        }

        bool connected() const{
            fd_set rfd;
            FD_ZERO(&rfd);
            FD_SET(m_socket, &rfd);
            timeval tv = { 0 };
            select(m_socket+1, &rfd, nullptr, nullptr, &tv);
            if (!FD_ISSET(m_socket, &rfd))
                return true;
            u_long n = 0;
            ioctlsocket(m_socket, FIONREAD, &n);
            return n != 0;
        }

        socket accept(const ip_address &addr) const {
            const sockaddr_in saddr = (const sockaddr_in)addr;
            int len = sizeof(sockaddr_in);
            socket_t accept = ::accept(m_socket, (sockaddr *)&saddr, &len);
            if (accept == -1) throw socket_exception("accept");
            socket sock = accept;
            return sock;
        }

        socket accept() const {
            socket_t accept = ::accept(m_socket, nullptr, nullptr);
            if (accept == -1) {
                if (LastError == WOULDBLOCK) throw unblocking_exception();
                throw socket_exception("accept");
            }
            socket sock = accept;
            return sock;
        }

        void connect(const ip_address &addr) const {
            sockaddr_in saddr = (const sockaddr_in )addr;
            if (::connect(m_socket, (sockaddr *)&saddr, sizeof(sockaddr_in)) < 0) {
                if (LastError == WOULDBLOCK) throw unblocking_exception();
                throw socket_exception("connect");
            }
        }

        int recv(char *ptr, int len) const {
            int res = ::recv(m_socket, ptr, len, 0);
            if (res < 0) {
                if (LastError == WOULDBLOCK) throw unblocking_exception();
                throw socket_exception("recv");
            }
            return res;
        }

        int send(const char *ptr, int len) const {
            int res = ::send(m_socket, ptr, len, 0);
            if (res < 0) {
                if (LastError == WOULDBLOCK) throw unblocking_exception();
                throw socket_exception("send");
            }
            return res;
        }

        void close() const {
            closesocket(m_socket);
        }
    };
}

#endif //ENHANCE_SOCKET_HPP
