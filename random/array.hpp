#ifndef ENHANCE_ARRAY_HPP
#define ENHANCE_ARRAY_HPP

#include <array>
#include "base.hpp"

namespace enhance::random {
    template <typename T>
    T choice(const std::vector<T> &vector){
        if (vector.empty()) throw random_exception("vector must not be empty");
        return vector[number(0, vector.size() - 1)];
    }

    template <typename T, size_t S>
    T choice(const std::array<T, S> &array){
        if (array.empty()) throw random_exception("array must not be empty");
        return array[number(0, array.size() - 1)];
    }

    template <typename T, size_t S>
    T choice(const T (&array)[S]){
        choice(array, S);
    }

    template <typename T>
    T choice(const T *array, size_t size){
        if (size == 0) throw random_exception("array must not be empty");
        return array[number<size_t>(0, size - 1)];
    }


    template <typename T>
    std::vector<T> array(T min, T max, size_t size){
        std::vector<T> vector(size);
        for (T &i : vector) i = number(min, max);
        return vector;
    }
}

#endif //ENHANCE_ARRAY_HPP
