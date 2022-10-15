#ifndef ENHANCE_HTTP_SERVER_HPP
#define ENHANCE_HTTP_SERVER_HPP

#include "../tcp/server.hpp"
#include "../string.hpp"
#include "../tcp/client.hpp"
#include <thread>
#include <functional>
#include <iostream>
#include <regex>
#include "http.hpp"

namespace enhance::http {
    class server {
        tcp::server m_server;
        std::map<std::string, std::function<void(const request &, response &)>> m_calls;
        std::function<void(const request &, response &)> m_not_found = [](const request &, response &res){
            res.status_code = 404;
            res.status = "Not Found";
            res.content = "<!doctype html><html><body><h1>404 Not Found</h1></body></html>";
        };

        std::function<void(const request &, response &)> m_bad_request = [](const request &, response &res){
            res.status_code = 400;
            res.status = "Bad Request";
            res.content = "<!doctype html><html><body><h1>400 Bad Request</h1></body></html>";
        };

    public:
        server() {
            m_server = {};
        }

        void on(std::string endPoint, std::function<void(const request &, response &)> function){
            if (!string::starts_with(endPoint, "/")) endPoint = "/" + endPoint;
            m_calls[endPoint] = std::move(function);
        }

        void on_not_found(const std::function<void(const request &, response &)> &function){
            m_not_found = function;
        }

        void on_bad_request(const std::function<void(const request &, response &)> &function){
            m_bad_request = function;
        }

        void start(int port = 80, bool wait = true){
            m_server.bind(socket::ip_address::any(port));
            m_server.listen();
            std::thread connect_thread([this]() {
                while (m_server.connected()) {
                    tcp::client client = (tcp::client)m_server.accept();
                    client.set_unblocking(true);
                    std::thread client_thread{[this](const tcp::client &client) {
                        std::regex http_request_regex("([A-Za-z]+) +(.*) +HTTP/[0-9][.][0-9]\r?\n((?:.|\r?\n)*)?\r?\n");
                        std::string message;
                        while (client.connected()) {
                            try {
                                char buff[1024];
                                while (true) {
                                    memset(buff, 0, 1024);
                                    int rec = client.recv(buff, 1024);
                                    message.insert(message.end(), buff, buff + rec);
                                }
                            } catch (socket::unblocking_exception &) {}
                            size_t find = message.find("\r\n\r\n");
                            if (find == std::string::npos) find = message.find("\n\n");

                            try{
                                request req = request::parse_request(message);
                                response res;
                                if (m_calls.count(req.endPoint)) {
                                    m_calls[req.endPoint](req, res);
                                    std::string sRes = res.make_response();
                                    client.send(sRes.c_str(), (int)sRes.length());
                                } else {
                                    m_not_found(req, res);
                                    std::string sRes = res.make_response();
                                    client.send(sRes.c_str(), (int)sRes.length());
                                }
                            } catch(std::exception &) {
                                response res{};
                                m_bad_request({}, res);
                                std::string sRes = res.make_response();
                                client.send(sRes.c_str(), (int)sRes.length());
                            }
                            client.close();
                        }
                    }, client};

                    client_thread.detach();
                }
            });

            if (wait) connect_thread.join();
            else connect_thread.detach();
        }
    };
}

#endif
