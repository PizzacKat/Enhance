#ifndef ENHANCE_GENERATORS_ARRAY_HPP
#define ENHANCE_GENERATORS_ARRAY_HPP

#include <vector>
#include "../functions.hpp"

namespace enhance::generators {
    template <typename T>
    std::vector<T> array(T(*pred)(), size_t size){
        std::vector<T> vector(size);
        for (T &p : vector) p = pred();
        return vector;
    }
}

#endif
