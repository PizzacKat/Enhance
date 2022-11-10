#ifndef ENHANCE_GET_HPP
#define ENHANCE_GET_HPP

#include <functional>

namespace enhance::getset {
    template <typename T, typename Getter = std::function<T()>>
    class get {
        Getter _get;
    public:
        get(Getter getter){ _get = getter; }
        operator T(){ return _get(); }
    };
}

#endif //ENHANCE_GET_HPP
