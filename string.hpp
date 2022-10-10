#ifndef ENHANCE_STRING_HPP
#define ENHANCE_STRING_HPP

#include <string>
#include <sstream>
#include <algorithm>

namespace enhance::string {
    template <typename T>
    std::string to_string(T val){
        std::stringstream ss;
        ss << val;
        return ss.str();
    }

    template <typename... Args>
    std::string format(std::string format, Args... args){
        std::vector<std::string> v_args;
        v_args.reserve(sizeof...(args));
        ((v_args.push_back(to_string(args))), ...);
        for (size_t i = 0; i < v_args.size(); i++){
            std::string n = std::to_string(i);
            size_t find;
            while ((find = format.find('{' + n + '}')) != std::string::npos) format.replace(find, n.length() + 2, v_args[i]);
        }
        return format;
    }

    std::vector<std::string> split(const std::string &str, const std::string& delim){
        if (str.empty()) return {""};
        std::vector<std::string> splits;
        size_t start = 0, end = 0;
        while ((start = str.find(delim, end)) != std::string::npos){
            splits.push_back(str.substr(end, start - end));
            end = start + delim.length();
        }
        splits.push_back(str.substr(end));
        return splits;
    }

    template <typename Iterator>
    bool starts_with(Iterator find_begin, Iterator find_end, Iterator str){
        for (; find_begin != find_end; str++, find_begin++) if (*find_begin != *str) return false;
        return true;
    }

    bool starts_with(const std::string &str, const std::string &start){
        if (str.length() < start.length()) return false;
        return starts_with(start.begin(), start.end(), str.begin());
    }

    bool ends_with(const std::string &str, const std::string &end){
        if (str.length() < end.length()) return false;
        return starts_with(end.rbegin(), end.rend(), str.rbegin());
    }

    std::string join(const std::vector<std::string>& str, const std::string& separator){
        std::string string;
        size_t s = str.size();
        for (size_t i = 0; i < s; i++){
            string += str[i];
            if (i != s-1) string += separator;
        }
        return string;
    }

    struct trim_t{
    public:
        virtual std::string operator()(std::string str) const = 0;
    };

    struct : trim_t {
        std::string operator()(std::string str) const override {
            str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char c){return !std::isspace(c);}));
            str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char c){return !std::isspace(c);}).base(), str.end());
            return str;
        }
    } trim;

    struct : trim_t {
        std::string operator()(std::string str) const override {
            str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char c){return !std::isspace(c);}));
            return str;
        }
    } trim_left;

    struct : trim_t {
        std::string operator()(std::string str) const override {
            str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char c){return !std::isspace(c);}).base(), str.end());
            return str;
        }
    } trim_right;

    std::string operator|(const std::string &str, const trim_t &t) {
        return t(str);
    }
}

#endif //ENHANCE_STRING_HPP