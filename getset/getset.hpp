#ifndef ENHANCE_GETTER_SETTER_H
#define ENHANCE_GETTER_SETTER_H

#include "get.hpp"
#include "set.hpp"

namespace enhance::getset {
    template <typename T, typename Getter = std::function<T()>, typename Setter = std::function<void(T)>>
    class getset : public get<T, Getter>, public set<T, Setter> {
    public:
        getset(Getter _getter, Setter _setter): get<T, Getter>(_getter), set<T, Setter>(_setter){}
        using get<T, Getter>::operator T;
        using set<T, Setter>::operator=;
    };
}

#endif //ENHANCE_GETTER_SETTER_H
