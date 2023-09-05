#ifndef ENHANCE_FUNCTIONS_HPP
#define ENHANCE_FUNCTIONS_HPP

#include <string>
#include <algorithm>

namespace enhance::functions {
    template <class Iterator, class T>
    auto accumulate(Iterator first, Iterator last, T init){
        while (first != last) init += *first++;
        return init;
    }

    template <class Iterator, class T>
    auto contract(Iterator first, Iterator last, T init){
        while (first != last) init -= *first++;
        return init;
    }

    template <class Iterator, class T>
    auto average(Iterator first, Iterator last, T init){
        size_t v = last - first;
        return functions::accumulate<Iterator, T>(first, last, init) / v;
    }

    template <class Iterator, class Predicate>
    Iterator find_if(Iterator first, Iterator last, Predicate pred){
        while (first != last && !pred(*first)) first++;
        return first;
    }

    template <class Iterator, class T>
    inline Iterator find(Iterator first, Iterator last, T val){
        return functions::find_if(first, last, [&val](const T &v) -> bool{return v == val;});
    }

    template <class Iterator, class T>
    inline ssize_t index_of(Iterator first, Iterator last, T val){
        Iterator f = find(first, last, val);
        return f == last ? -1 : f - first;
    }

    template <class Iterator, class Predicate>
    inline size_t index_if(Iterator first, Iterator last, Predicate pred){
        Iterator f = find_if(first, last, pred);
        return f == last ? -1 : f - first;
    }

    template <class Iterator, class Predicate>
    inline bool contains_if(Iterator first, Iterator last, Predicate pred){
        return functions::find_if(first, last, pred) != last;
    }

    template <class Iterator, class T>
    inline bool contains(Iterator first, Iterator last, T val){
        return functions::find(first, last, val) != last;
    }
}

#endif //ENHANCE_FUNCTIONS_HPP
