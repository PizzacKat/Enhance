#ifndef ENHANCE_RANDOM_ARRAY_HPP
#define ENHANCE_RANDOM_ARRAY_HPP

#include <array>
#include "base.hpp"

namespace enhance::random {
    template <typename T>
    T choice(const std::vector<T> &vector){
        if (vector.empty()) throw random_exception("vector must not be empty");
        return vector[number<size_t>(0, vector.size() - 1)];
    }

    template <typename T, size_t S>
    T choice(const std::array<T, S> &array){
        if (array.empty()) throw random_exception("array must not be empty");
        return array[number<size_t>(0, array.size() - 1)];
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

    template <typename T>
    std::vector<T> shuffle(const std::vector<T> &vec){
        std::vector<T> s;
        s.resize(vec.size());
        for (size_t i = 0; i < vec.size(); i++){
            s[number(0, vec.size())] = vec[i];
        }
    }
}

#endif //ENHANCE_ARRAY_HPP
