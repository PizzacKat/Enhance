#ifndef ENHANCE_HTTP_SERVER_HPP
#define ENHANCE_HTTP_SERVER_HPP

#include "../tcp/server.hpp"
#include "../string.hpp"
#include "../tcp/client.hpp"
#include <thread>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <iostream>
#include <regex>

namespace enhance::http {
    class response {
    public:
        unsigned short status_code = 200;
        std::string status = "OK";
        std::map<std::string, std::string> headers = {};
        std::string content;

        std::string make_response(){
            std::string response = string::format("HTTP/1.1 {0} {1}\n", status_code, status);
            for (const std::pair<const std::string, std::string> &header : headers){
                response += string::format("{0}: {1}\n", header.first, header.second);
            }
            response += "\n" + content;
            return response;
        }

        static std::string read_file(const char *filename){
            std::ifstream file(filename);
            if (!file.is_open()) throw std::exception();
            std::string line, content;
            while (std::getline(file, line))
                content += line + "\n";
            return content;
        }
    };

    class request {
    public:
        std::string type;
        std::string endPoint;
        std::map<std::string, std::string> headers;
        std::string content;

        static std::map<std::string, std::string> parse_headers(const std::string& str){
            std::map<std::string, std::string> headers{};
            std::vector<std::string> lines = string::split(str, "\n");

            for (const std::string &line : lines){
                std::vector<std::string> args = string::split(line, ":");
                if (args.size() < 2) continue;
                headers[args[0] | string::trim] = args[1] | string::trim;
            }

            return headers;
        }
    };

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

        void on_not_found(std::function<void(const request &, response &)> function){

        }

        void start(int port = 80){
            m_server.bind(socket::ip_address::any(port));
            m_server.listen();
            while (m_server.connected()){
                tcp::client client = m_server.accept();
                client.set_unblocking(true);
                std::thread client_thread{[this](tcp::client client){
                    std::regex http_request_regex("([A-Za-z]+) +(.*) +HTTP/[0-9][.][0-9]\r?\n((?:.|\r?\n)*)?\r?\n");
                    std::string message;
                    while (client.connected()) {
                        try{
                            char buff[1024];
                            while (true) {
                                memset(buff, 0, 1024);
                                int rec = client.recv(buff, 1024);
                                message.insert(message.end(), buff, buff + rec);
                            }
                        }catch (socket::unblocking_exception &){}
                        size_t find = message.find("\r\n\r\n");
                        if (find == std::string::npos) find = message.find("\n\n");

                        std::smatch sm;
                        std::string reg = message.substr(0, find+1);
                        if (find != std::string::npos && std::regex_search(reg, sm, http_request_regex)){
                            std::string type = sm[1];
                            std::string endPoint = sm[2];
                            std::string headers = sm[3];
                            std::string content = message.substr(find + 1);
                            response res{};
                            request req{};
                            req.endPoint = endPoint;
                            req.type = type;
                            req.content = content;
                            req.headers = request::parse_headers(headers);
                            if (m_calls.count(endPoint)) {
                                m_calls[endPoint](req, res);
                                std::string sRes = res.make_response();
                                client.send(sRes.c_str(), sRes.length());
                            }
                            else {
                                m_not_found(req, res);
                                std::string sRes = res.make_response();
                                client.send(sRes.c_str(), sRes.length());
                            }
                        }else{
                            response res{};
                            m_bad_request({}, res);
                            std::string sRes = res.make_response();
                            client.send(sRes.c_str(), sRes.length());
                        }
                        client.close();
                    }
                }, client};

                client_thread.detach();
            }
        }
    };
}

#endif
