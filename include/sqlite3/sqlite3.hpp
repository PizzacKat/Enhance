#ifndef ENHANCE_SQLITE3_HPP
#define ENHANCE_SQLITE3_HPP

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include "sqlite3.h"

namespace enhance::sqlite {
    class exception : public std::exception {
        const char *message;
    public:
        explicit exception(const char *message) : message(message){}

        [[nodiscard]] const char * what() const noexcept override{
            return message;
        }
    };

    class row {
        std::string m_value;
        friend class database;
        friend class statement;
        explicit row(std::string value) : m_value(std::move(value)){}
    public:
        template <typename T>
        [[nodiscard]] T get() const{
            if constexpr(std::is_integral_v<T> && std::is_unsigned_v<T>){
                return (T)std::stoull(m_value);
            }
            if constexpr(std::is_integral_v<T> && std::is_signed_v<T>){
                return (T)std::stoll(m_value);
            }
            if constexpr(std::is_floating_point_v<T>){
                return (T)std::stod(m_value);
            }
            if constexpr(std::is_same_v<T, std::string>){
                return m_value;
            }
        }
    };

    class result : public std::map<std::string, std::vector<row>> {
};

class command {
    std::string m_str;
    friend class database;
public:
    command(){
        m_str = "";
    }

    explicit command(const char *str){
        m_str = str;
    }

    [[maybe_unused]] static command fromString(const std::string &str){
        command cmd;
        cmd.m_str = str;
        return cmd;
    }

    [[maybe_unused]] static command fromFile(const std::string &filename){
        command cmd;
        std::ifstream stream{filename};
        std::string line;
        while (std::getline(stream, line)){
            cmd.m_str += line;
        }
        cmd.m_str.erase(cmd.m_str.end() - 1);
        return cmd;
    }
};

class statement {
    sqlite3_stmt *m_stmt;
    friend class database;
    explicit statement(sqlite3_stmt *stmt) : m_stmt(stmt){

    }
public:
    template <typename T>
    void bind_parameter(const T& param, int index){
        int res;
        if constexpr(std::is_same_v<int64_t, T>){
            res = sqlite3_bind_int64(m_stmt, index, param);
        }
        if constexpr(std::is_same_v<int32_t, T>){
            res = sqlite3_bind_int(m_stmt, index, param);
        }
        if constexpr(std::is_same_v<double, T>){
            res = sqlite3_bind_double(m_stmt, index, param);
        }
        if constexpr(std::is_same_v<float, T>){
            res = sqlite3_bind_double(m_stmt, index, (double)param);
        }
        if constexpr(std::is_same_v<std::string, T>){
            res = sqlite3_bind_text(m_stmt, index, param.c_str(), param.length(), SQLITE_STATIC);
        }
        if (res != SQLITE_OK){
            throw exception(sqlite3_errstr(res));
        }
    }

    result execute(){
        int r;
        result res;
        do {
            r = sqlite3_step(m_stmt);
            if (r == SQLITE_ROW) res[sqlite3_column_name(m_stmt, 0)].push_back((class row){(const char *)sqlite3_column_text(m_stmt, 0)});
            if (r != SQLITE_ROW && r != SQLITE_DONE) throw exception("Couldn't execute statement.");
        }while (r == SQLITE_ROW);
        return res;
    }

    void reset(){
        sqlite3_reset(m_stmt);
    }

    void finalize(){
        sqlite3_finalize(m_stmt);
    }
};

class database {
    sqlite3 *m_conn{};

    static inline int exec(void *res, int n, char **values, char **names){
        result &r = *(result*)res;
        for (int row = 0; row < n; row++){
            r[names[row]].push_back((class row){values[row]});
        }
        return SQLITE_OK;
    }
    explicit database(sqlite3 *conn){
        m_conn = conn;
    }
public:
    explicit database(const char *filename){
        int res = sqlite3_open(filename, &m_conn);
        if (res != SQLITE_OK) throw exception(sqlite3_errstr(res));
    }

    statement create_statement(const command& cmd){
        sqlite3_stmt* stmt;
        int res = sqlite3_prepare_v2(m_conn, cmd.m_str.c_str(), (int)cmd.m_str.length(), &stmt, nullptr);
        if (res != SQLITE_OK) throw exception(sqlite3_errstr(res));
        return statement{stmt};
    }

    result execute(const command& cmd){
        result res;
        char *err;
        if (sqlite3_exec(m_conn, cmd.m_str.c_str(), exec, &res, &err) != SQLITE_OK) throw exception(err);
        return res;
    }

    void close(){
        sqlite3_close(m_conn);
    }
};
}

#endif //ENHANCE_SQLITE3_HPP
