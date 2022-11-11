#ifndef ENHANCE_HTTP_CLIENT_HPP
#define ENHANCE_HTTP_CLIENT_HPP

#include "../tcp/server.hpp"
#include "../string.hpp"
#include "../tcp/client.hpp"
#include <thread>
#include <functional>
#include <iostream>
#include <regex>
#include "http.hpp"

namespace enhance::http {
    class client {
        tcp::client m_client;
    public:
        client() {
            m_client = {};
        }

        response request(const socket::ip_address &address, const request &request) {
            m_client.connect(address);
            std::string req = request.make_request();
            m_client.send(req.c_str(), (int)req.length());
            m_client.set_unblocking(true);
            std::string res;
            char buff[512];
            while (m_client.connected()) {
                try {
                    memset(buff, 0, 512);
                    int rec = m_client.recv(buff, 512);
                    res.insert(res.end(), buff, buff + rec);
                    std::cout << buff << "\n";
                } catch (socket::unblocking_exception &) {}
            }
            m_client.close();
            return response::parse_response(res);
        }
    };
}

#endif