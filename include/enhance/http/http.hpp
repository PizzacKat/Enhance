#ifndef ENHANCE_HTTP_BASE_HPP
#define ENHANCE_HTTP_BASE_HPP

#include <map>
#include <string>
#include <fstream>
#include "../string.hpp"

namespace enhance::http {
    class response {
        static inline std::regex http_response_regex{"HTTP/[0-9]+[.][0-9]+ ([0-9]+) (.*)\r?\n((?:.|\r?\n)*)?\r?\n"};
    public:
        unsigned short status_code = 200;
        std::string status = "OK";
        std::map<std::string, std::string> headers = {};
        std::string content;

        std::string make_response() const{
            std::string response = string::format("HTTP/1.1 {0} {1}\n", status_code, status);
            for (const std::pair<const std::string, std::string> &header : headers){
                response += string::format("{0}: {1}\n", header.first, header.second);
            }
            response += "\n" + content;
            return response;
        }

        static response parse_response(const std::string &str){
            std::cout << str << "\n";
            size_t find = str.find("\r\n\r\n");
            if (find == std::string::npos) find = str.find("\n\n");

            std::smatch sm;
            std::string reg = str.substr(0, find + 1);
            if (find != std::string::npos && std::regex_search(reg, sm, http_response_regex)) {
                std::string status_code = sm[1];
                std::string status = sm[2];
                std::string headers = sm[3];
                std::string content = str.substr(find + 1);
                response req{};
                req.status_code = (short)std::stoul(status_code);
                req.status = status;
                req.content = content;
                req.headers = parse_headers(headers);
                return req;
            }
            throw std::exception();
        }

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
        static inline std::regex http_request_regex{"([A-Za-z]+) +(.*) +HTTP/[0-9][.][0-9]\r?\n((?:.|\r?\n)*)?\r?\n"};
    public:
        std::string type;
        std::string endPoint;
        std::map<std::string, std::string> headers;
        std::string content;

        std::string make_request() const{
            std::string request = string::format("{0} {1} HTTP/1.1\n", type, endPoint);
            for (const std::pair<const std::string, std::string> &header : headers){
                request += string::format("{0}: {1}\n", header.first, header.second);
            }
            request += "\n" + content;
            return request;
        }

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

        static request parse_request(const std::string& str){
            size_t find = str.find("\r\n\r\n");
            if (find == std::string::npos) find = str.find("\n\n");

            std::smatch sm;
            std::string reg = str.substr(0, find + 1);
            if (find != std::string::npos && std::regex_search(reg, sm, http_request_regex)) {
                std::string type = sm[1];
                std::string endPoint = sm[2];
                std::string headers = sm[3];
                std::string content = str.substr(find + 1);
                request req{};
                req.endPoint = endPoint;
                req.type = type;
                req.content = content;
                req.headers = parse_headers(headers);
                return req;
            }
            throw std::exception();
        }
    };
}

#endif //ENHANCE_HTTP_BASE_HPP
