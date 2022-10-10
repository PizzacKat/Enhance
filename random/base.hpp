#ifndef ENHANCE_RANDOM_BASE_HPP
#define ENHANCE_RANDOM_BASE_HPP

#include <type_traits>
#include <random>

namespace enhance::random {
    class random_exception : std::exception {
    private:
        const char *m_what;
    public:
        explicit random_exception(const char *what){
            m_what = what;
        }

        [[nodiscard]] const char * what() const noexcept override {
            return m_what;
        }
    };

    std::random_device dev{};
    std::mt19937 rng(dev());

    template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
    T number(T min, T max){
        return std::uniform_int_distribution<T>(min, max)(rng);
    }

    template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
    T number(T min, T max){
        return std::uniform_real_distribution<T>(min, max)(rng);
    }
}

#endif //ENHANCE_BASE_HPP
