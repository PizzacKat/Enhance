#ifndef ENHANCE_SETTER_H
#define ENHANCE_SETTER_H

#include <functional>

namespace enhance::getset {
    template <typename T, typename Setter = std::function<void(T)>>
    class set {
        Setter _set;
    public:
        set(Setter setter){ _set = setter; }
        set &operator =(T t){ _set(t); }
    };
}

#endif //ENHANCE_SETTER_H
