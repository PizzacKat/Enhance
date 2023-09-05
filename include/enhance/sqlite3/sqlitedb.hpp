#ifndef ENHANCE_SQLITEDB_H
#define ENHANCE_SQLITEDB_H

#include <sqlite3.h>
#include <string>
#include "sqlite_statement.hpp"
#include "sqlite_exception.hpp"

namespace enhance::sqlite {
    class database : public ReferenceCounter {
        sqlite3 *m_handle;
    public:
        database() {
            m_handle = nullptr;
        }

        explicit database(const std::string &filename) : database() {
            open(filename);
        }

        void open(const std::string &filename) {
            int res = sqlite3_open(filename.c_str(), &m_handle);
            if (res != SQLITE_OK){
                sqlite3_close(m_handle);
                m_handle = nullptr;
                throw_generic_error<open_exception>(res);
            }
        }

        [[nodiscard]] bool open() const{
            return m_handle != nullptr;
        }

        statement prepare(const std::string &statement){
            if (!open()) throw not_opened_exception();
            return {m_handle, statement};
        }

        template <typename... Args>
        std::map<std::string, std::vector<value>> execute(const std::string &statement, Args... args){
            class statement stmt = prepare(statement);
            int i = 1;
            ((stmt.bind(args, i++)), ...);
            return stmt.values();
        }

        void close() {
            if (open())
                sqlite3_close(m_handle);
            m_handle = nullptr;
        }

        ~database() {
            if (last())
                close();
        }
    };
}

#endif //ENHANCE_SQLITEDB_H
