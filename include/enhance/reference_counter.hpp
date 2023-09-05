#ifndef ENHANCE_REFERENCE_COUNTER_HPP
#define ENHANCE_REFERENCE_COUNTER_HPP

#include <cstddef>
namespace enhance {
    class reference_counter {
        size_t *m_count;

        void add() {
            (*m_count)++;
        }

        void sub() {
            (*m_count)--;
        }

    public:
        [[nodiscard]] bool last() const {
            return *m_count <= 1;
        }

        reference_counter() {
            m_count = new size_t(0);
            add();
        }

        reference_counter(const reference_counter &reference_counter) {
            m_count = reference_counter.m_count;
            add();
        }

        reference_counter &operator=(const reference_counter &reference_counter) {
            if (this == &reference_counter) return *this;
            sub();
            if (*m_count == 0) delete m_count;
            m_count = reference_counter.m_count;
            add();
            return *this;
        }

        ~reference_counter() {
            sub();
            if (*m_count == 0) delete m_count;
        }
    };
}

#endif //ENHANCE_REFERENCE_COUNTER_HPP
