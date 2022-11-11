#ifndef ENHANCE_SOCKET_BASE_HPP
#define ENHANCE_SOCKET_BASE_HPP

#include <winsock2.h>
#include <ws2tcpip.h>
#define LastError WSAGetLastError()
#define WOULDBLOCK WSAEWOULDBLOCK
#include <exception>

namespace enhance::socket {
    class socket_exception : public std::exception {
    protected:
        std::string m_what;
    public:
        explicit socket_exception(const char *what) : m_what(what){

        }
        [[nodiscard]] const char *what() const noexcept override {
            return m_what.c_str();
        }
    };

    class unblocking_exception : public std::exception {

    };

    class ip_address {

    public:
        in_addr address{};
        unsigned short port = 80;

        ip_address(){
            memset(&address, 0, sizeof(in_addr));
        }

        static ip_address parse(const char *address, unsigned short port){
            ip_address _address{};
            inet_pton(AF_INET, address, &_address.address);
            _address.port = port;
            return _address;
        }

        static ip_address any(unsigned short port){
            ip_address _address{};
            _address.address.s_addr = ADDR_ANY;
            _address.port = port;
            return _address;
        }

        explicit operator const sockaddr_in() const{
            sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            addr.sin_addr = address;
            addr.sin_port = htons(port);
            addr.sin_family = AF_INET;
            return addr;
        }
    };

    struct win_init {
        win_init(){
            WSADATA data;
            if (WSAStartup(MAKEWORD(2, 2), &data) != 0) throw socket_exception("WSAStartup");
        }
        ~win_init(){
            WSACleanup();
        }
    } _win_init;
}

#endif //ENHANCE_BASE_HPP
