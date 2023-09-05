#ifndef ENHANCE_SQLITE_EXCEPTION_H
#define ENHANCE_SQLITE_EXCEPTION_H

#include <string>
#include <exception>
#include <sqlite3.h>

namespace enhance::sqlite {
    template <typename T>
    void throw_generic_error(int errcode){
        throw T(sqlite3_errstr(errcode));
    }

    class open_exception : public std::exception{
        std::string m_what;
    public:
        explicit open_exception(const std::string &description){
            m_what = "Error opening sqlite file: " + description;
        }

        open_exception(){
            m_what = "Error opening sqlite file";
        }

        [[nodiscard]] const char *what() const noexcept override{
            return m_what.c_str();
        }
    };

    class not_opened_exception : public std::exception {
    public:
        [[nodiscard]] const char * what() const noexcept override{
            return "Database file is not opened";
        }
    };

    class prepare_exception : public std::exception{
        std::string m_what;
    public:
        explicit prepare_exception(const std::string &description){
            m_what = "Error preparing statement: " + description;
        }

        prepare_exception(){
            m_what = "Error preparing statement";
        }

        [[nodiscard]] const char *what() const noexcept override{
            return m_what.c_str();
        }
    };

    class not_prepared_exception : public std::exception {
    public:
        [[nodiscard]] const char * what() const noexcept override{
            return "Statement is not prepared";
        }
    };

    class invalid_value_type_exception : public std::exception{
    public:
        [[nodiscard]] const char *what() const noexcept override{
            return "Invalid conversion type given";
        }
    };

    class step_exception : public std::exception{
        std::string m_what;
    public:
        explicit step_exception(const std::string &description){
            m_what = "Error stepping statement: " + description;
        }

        step_exception(){
            m_what = "Error stepping statement";
        }

        [[nodiscard]] const char *what() const noexcept override{
            return m_what.c_str();
        }
    };

    class reset_exception : public std::exception{
        std::string m_what;
    public:
        explicit reset_exception(const std::string &description){
            m_what = "Error resetting statement: " + description;
        }

        reset_exception(){
            m_what = "Error resetting statement";
        }

        [[nodiscard]] const char *what() const noexcept override{
            return m_what.c_str();
        }
    };
}

#endif //ENHANCE_SQLITE_EXCEPTION_H
