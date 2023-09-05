#ifndef ENHANCE_SQLITE_VALUE_H
#define ENHANCE_SQLITE_VALUE_H

#include <sqlite3.h>
#include <vector>
#include <map>
#include "sqlite_exception.hpp"
#include <ReferenceCounter.h>

namespace enhance::sqlite {
    class value : protected ReferenceCounter {
        sqlite3_value *m_handle;
    public:
        value(){
            m_handle = nullptr;
        }

        explicit value(sqlite3_value *handle){
            m_handle = sqlite3_value_dup(handle);
        }

        value &operator=(const value &value){
            if (&value == this) return *this;
            if (last()) free();
            m_handle = value.m_handle;
            return *this;
        }

        [[nodiscard]] bool valid() const{
            return m_handle != nullptr;
        }

        template <typename T>
        [[nodiscard]] T get() const{
            switch (sqlite3_value_type(m_handle)){
                case SQLITE_INTEGER:
                    if constexpr (std::is_convertible_v<int64_t, T>)
                        return (T)sqlite3_value_int64(m_handle);
                    if constexpr (std::is_convertible_v<int, T>)
                        return (T)sqlite3_value_int(m_handle);
                    break;
                case SQLITE_FLOAT:
                    if constexpr(std::is_convertible_v<double, T>)
                        return (T)sqlite3_value_double(m_handle);
                    break;
                case SQLITE_TEXT:
                    if constexpr (std::is_convertible_v<const char *, T>)
                        return (T)(const char *)sqlite3_value_text(m_handle);
                    break;
                case SQLITE_BLOB:
                    if constexpr(std::is_convertible_v<const void *, T>)
                        return (T)sqlite3_value_blob(m_handle);
                    return *(const T *)sqlite3_value_blob(m_handle);
                case SQLITE_NULL:
                    if constexpr(std::is_convertible_v<std::nullptr_t, T>)
                        return (T)nullptr;
                    break;
            }
            throw invalid_value_type_exception();
        }

        [[nodiscard]] size_t size() const{
            return sqlite3_value_bytes(m_handle);
        }

        void free(){
            if (valid())
                sqlite3_value_free(m_handle);
            m_handle = nullptr;
        }

        ~value(){
            if (last()) free();
        }
    };
}

#endif //ENHANCE_SQLITE_VALUE_H
