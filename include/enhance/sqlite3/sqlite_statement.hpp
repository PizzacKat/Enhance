#ifndef ENHANCE_SQLITE_STATEMENT_H
#define ENHANCE_SQLITE_STATEMENT_H

#include <sqlite3.h>
#include <string>
#include "sqlite_exception.hpp"
#include <optional>
#include "sqlite_value.hpp"
#include <vector>
#include <map>

namespace enhance::sqlite {
    class statement : protected ReferenceCounter {
        sqlite3_stmt *m_handle;
    public:
        statement(){
            m_handle = nullptr;
        }

        statement(statement &&statement) noexcept{
            m_handle = statement.m_handle;
            statement.m_handle = nullptr;
        }

        explicit statement(sqlite3_stmt *handle){
            m_handle = handle;
        }

        statement(sqlite3 *db, const std::string &statement): sqlite::statement(){
            int res = sqlite3_prepare_v2(db, statement.c_str(), (int)statement.length(), &m_handle, nullptr);
            if (res != SQLITE_OK){
                sqlite3_finalize(m_handle);
                m_handle = nullptr;
                throw_generic_error<prepare_exception>(res);
            }
        }

        template <typename T>
        void bind(const T &value, int i){
            if constexpr(std::is_integral_v<T>)
                sqlite3_bind_int64(m_handle, i, value);
            else if constexpr(std::is_floating_point_v<T>)
                sqlite3_bind_double(m_handle, i, value);
            else if constexpr(std::is_null_pointer_v<T>)
                sqlite3_bind_null(m_handle, i);
            else if constexpr(std::is_same_v<T, std::string>)
                sqlite3_bind_text64(m_handle, i, value.c_str(), value.length(), SQLITE_TRANSIENT, SQLITE_UTF8);
            else if constexpr(std::is_same_v<T, const char *>)
                sqlite3_bind_text(m_handle, i, value, -1, SQLITE_TRANSIENT);
            else sqlite3_bind_blob(m_handle, i, (void *)&value, sizeof(T), SQLITE_TRANSIENT);
        }

        bool reset(){
            if (!prepared()) throw not_prepared_exception();
            int res = sqlite3_reset(m_handle);
            if (res != SQLITE_OK)
                throw_generic_error<reset_exception>(res);
        }

        bool step(){
            if (!prepared()) throw not_prepared_exception();
            int res = sqlite3_step(m_handle);
            if (res != SQLITE_DONE && res != SQLITE_ROW)
                throw_generic_error<step_exception>(res);
            return res == SQLITE_DONE;
        }

        [[nodiscard]] std::vector<value> row() const{
            if (!prepared()) throw not_prepared_exception();
            std::vector<value> row;
            for (int i = 0; i < sqlite3_column_count(m_handle); i++) row.emplace_back(sqlite3_column_value(m_handle, i));
            return row;
        }

        void run(){
            if (!prepared()) throw not_prepared_exception();
            while (!step());
        }

        std::map<std::string, std::vector<value>> values(){
            if (!prepared()) throw not_prepared_exception();
            std::map<std::string, std::vector<value>> values;
            while (!step()) {
                auto row = statement::row();
                for (int i = 0; i < row.size(); i++) values[sqlite3_column_name(m_handle, i)].push_back(row[i]);
            }
            return values;
        }

        [[nodiscard]] bool prepared() const{
            return m_handle != nullptr;
        }

        void finalize(){
            if (prepared())
                sqlite3_finalize(m_handle);
            m_handle = nullptr;
        }

        ~statement(){
            if (last()) finalize();
        }
    };
}

#endif //ENHANCE_SQLITE_STATEMENT_H
