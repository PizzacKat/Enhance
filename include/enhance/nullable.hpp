#ifndef ENHANCE_NULLABLE_HPP
#define ENHANCE_NULLABLE_HPP

#include <exception>

namespace enhance {
    class empty_value_exception : std::exception {

    };

    template<typename T>
    class nullable {
        T *m_value;
        bool m_has_value;

    public:
        nullable(const nullable &nullable) {
            m_has_value = nullable.m_has_value;
            if (m_has_value) m_value = new T(*nullable.m_value);
            else m_value = nullptr;
        }

        nullable(T value) {
            m_has_value = true;
            m_value = new T(value);
        }

        nullable(std::nullptr_t) : nullable() {}

        nullable() {
            m_has_value = false;
            m_value = nullptr;
        }

        nullable &operator=(std::nullptr_t) {
            if (m_has_value) delete m_value;
            m_has_value = false;
            m_value = nullptr;
            return *this;
        }

        nullable &operator=(T value) {
            if (m_has_value) delete m_value;
            m_has_value = true;
            m_value = new T(value);
            return *this;
        }

        T &value() {
            if (!m_has_value) throw empty_value_exception();
            return *m_value;
        }

        bool has_value() {
            return m_has_value;
        }

        ~nullable() {
            if (m_has_value) delete m_value;
        }
    };
}

#endif //ENHANCE_NULLABLE_HPP
