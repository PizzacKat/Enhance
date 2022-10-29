#ifndef ENHANCE_LINQ_HPP
#define ENHANCE_LINQ_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <memory>

namespace enhance::linq {
    class exception : public std::exception {
        const char *m_what;
    public:
        explicit exception(const char *what) : m_what(what) {}
        [[nodiscard]] const char * what() const noexcept override {return m_what;}
    };

    template <typename T, typename Allocator = std::allocator<T>>
    class enumerable {
    protected:
        Allocator m_allocator = {};
        T *m_data = nullptr;
        size_t m_size = 0;
    public:
        enumerable(){
            m_size = 0;
            m_data = nullptr;
        }

        enumerable(const enumerable &enumerable){
            m_size = enumerable.size();
            m_data = m_allocator.allocate(size());
            for (size_t i = 0; i < size(); i++) new(m_data + i) T(enumerable.m_data[i]);
        }

        explicit enumerable(size_t size){
            m_size = size;
            m_data = m_allocator.allocate(size);
            for (size_t i = 0; i < size; i++) new(m_data + i) T();
        }

        template <typename Iterator>
        enumerable(Iterator start, Iterator finish){
            m_size = finish - start;
            m_data = m_allocator.allocate(size());
            auto datait = begin();
            for (auto it = start; it < finish; it++, datait++) *datait = *it;
        }

        enumerable &operator =(const enumerable &enumerable){
            if (this == &enumerable) return *this;
            m_allocator.deallocate(m_data, size());
            m_size = enumerable.size();
            m_data = m_allocator.allocate(size());
            for (size_t i = 0; i < size(); i++) new(m_data + i) T(enumerable.m_data[i]);
            return *this;
        }

        [[nodiscard]] size_t size() const{ return m_size; }
        virtual T &operator[](size_t i){ return m_data[i]; }
        virtual const T &operator[](size_t i) const{ return m_data[i]; }
        virtual T *begin(){ return m_data; }
        virtual const T *begin() const{ return m_data; }
        virtual T *end(){ return m_data + size(); }
        virtual const T *end() const{ return m_data + size(); }

        std::reverse_iterator<T *>rbegin(){ return std::make_reverse_iterator(end()); }
        const std::reverse_iterator<const T *> rbegin() const{ return std::make_reverse_iterator(end()); }
        std::reverse_iterator<T *>rend(){ return std::make_reverse_iterator(begin()); }
        const std::reverse_iterator<const T *> rend() const{ return std::make_reverse_iterator(begin()); }

        template <typename Result, typename Predicate>
        enumerable<Result> select(Predicate pred) const{
            enumerable<Result> array(size());
            auto it = this->begin();
            auto resit = array.begin();
            for (; it < this->end(); it++, resit++) *resit = pred(*it, it - this->begin());
            return array;
        }

        enumerable<T> concat(const enumerable<T> &e) const{
            enumerable<T> array(size() + e.size());
            auto resit = array.begin();
            for (auto it = begin(); it < end(); it++, resit++) *resit = *it;
            for (auto it = e.begin(); it < e.end(); it++, resit++) *resit = *it;
            return array;
        }

        template <typename Result, typename Second, typename Predicate>
        enumerable<Result> zip(const enumerable<Second> &e, Predicate pred) const{
            if (e.size() != size()) throw exception("Array size is not the same :(");
            enumerable<Result> array(size());
            auto it1 = begin();
            auto it2 = e.begin();
            auto resit = array.begin();
            for (; it1 < end(); it1++, it2++, resit++) *resit = pred(*it1, *it2);
            return array;
        }

        template <typename Predicate>
        enumerable<T> where(Predicate pred) const{
            enumerable<T> array(size());
            auto resit = array.begin();
            for (auto it = this->begin(); it < this->end(); it++) if (pred(*it)) *resit++ = *it;
            enumerable<T> newarr(resit - array.begin());
            for (auto it1 = array.begin(), it2 = newarr.begin(); it2 < newarr.end(); it1++, it2++) *it2 = *it1;
            return newarr;
        }

        enumerable<T> skip(size_t elements) const{
            if (begin() + elements >= end()) throw exception("Can't skip, too much :(");
            return enumerable<T>(this->begin() + elements, this->end());
        }

        enumerable<T> skip_last(size_t elements) const{
            if (begin() >= end() - elements) throw exception("Can't skip, too much :(");
            return enumerable<T>(this->begin(), this->end() - elements);
        }

        enumerable<T> take(size_t elements) const{
            if (begin() + elements >= end()) throw exception("Can't take, too much :(");
            return enumerable<T>(this->begin(), this->begin() + elements);
        }

        enumerable<T> take_last(size_t elements) const{
            if (this->begin() >= this->end() - elements) throw exception("Can't take, too much :(");
            return enumerable<T>(this->end() - elements, this->end());
        }

        enumerable<T> reverse() const{
            return enumerable<T>{this->rbegin(), this->rend()};
        }

        template <typename Result, typename Accumulator, typename Predicate, typename Selector>
        Result aggregate(Accumulator &&accumulate, Predicate pred, Selector sel) const{
            for (auto it = this->begin(); it < this->end(); it++) accumulate = pred(accumulate, *it);
            return sel(accumulate);
        }

        template <typename Accumulator, typename Predicate>
        Accumulator aggregate(Accumulator &&accumulate, Predicate pred) const{
            for (auto it = this->begin(); it < this->end(); it++) accumulate = pred(accumulate, *it);
            return accumulate;
        }

        template <typename Predicate>
        T aggregate(Predicate pred) const{
            if (this->begin() == this->end()) throw exception("No elements :(");
            T accumulate = *this->begin();
            for (auto it = this->begin()+1; it < this->end(); it++) accumulate = pred(accumulate, *it);
            return accumulate;
        }

        template <typename Predicate = bool(const T&)>
        T &first(Predicate pred = [](const T &){return true;}) const{
            for (auto it = this->begin(); it < this->end(); it++) if (pred(*it)) return *it;
            throw exception("There was no first :(");
        }

        template <typename Predicate = bool(const T&)>
        T &first_or_default(Predicate pred = [](const T &){return true;}, T def = {}) const{
            for (auto it = this->begin(); it < this->end(); it++) if (pred(*it)) return *it;
            return def;
        }

        template <typename Predicate = bool(const T &)>
        T &last(Predicate pred = [](const T &){return true;}) const{
            for (auto it = this->rbegin(); it < this->rend(); it++) if (pred(*it)) return *it;
            throw exception("There was no last :(");
        }

        template <typename Predicate = bool(const T &)>
        T &last_or_default(Predicate pred = [](const T &){return true;}, T def = {}) const{
            for (auto it = this->rbegin(); it < this->rend(); it++) if (pred(*it)) return *it;
            return def;
        }

        template <typename Result, typename Predicate = Result(const T &)>
        Result min(Predicate pred = [](const T &a){return a;}) const{
            if (this->begin() == this->end()) throw exception("No elements :(");
            Result min = pred(*this->begin());
            for (auto it = this->begin()+1; it < this->end(); it++) {
                Result val = pred(*it);
                if (val < min) min = val;
            }
            return min;
        }

        T min() const{
            if (this->begin() == this->end()) throw exception("No elements :(");
            T min = *this->begin();
            for (auto it = this->begin()+1; it < this->end(); it++) if (*it < min) min = *it;
            return min;
        }

        template <typename Result, typename Predicate = Result(const T &)>
        Result max(Predicate pred = [](const T &a){return a;}) const{
            if (this->begin() == this->end()) throw exception("No elements :(");
            Result max = pred(*this->begin());
            for (auto it = this->begin()+1; it < this->end(); it++) {
                Result val = pred(*it);
                if (val > max) max = val;
            }
            return max;
        }

        T max() const{
            if (this->begin() == this->end()) throw exception("No elements :(");
            T max = *this->begin();
            for (auto it = this->begin()+1; it < this->end(); it++) if (*it > max) max = *it;
            return max;
        }

        bool contains(T value) const{
            for (auto it = this->begin(); it < this->end(); it++) if (*it == value) return true;
            return false;
        }

        template <typename Predicate = bool(const T &)>
        bool any(Predicate pred = [](const T &){return true;}) const{
            for (auto it = this->begin(); it < this->end(); it++) if (pred(*it)) return true;
            return false;
        }

        template <typename Predicate>
        bool all(Predicate pred) const{
            for (auto it = this->begin(); it < this->end(); it++) if (!pred(*it)) return false;
            return true;
        }

        template <typename Predicate = bool(const T &)>
        size_t count(Predicate pred = [](const T &){return true;}) const{
            size_t count = 0;
            for (auto it = this->begin(); it < this->end(); it++) if (pred(*it)) count++;
            return count;
        }

        ~enumerable(){
            m_allocator.deallocate(m_data, size());
        }
    };

    template <typename T, typename Allocator = std::allocator<T>>
    class array : public enumerable<T, Allocator> {
        typedef enumerable<T, Allocator> Base;
    public:
        using Base::enumerable;

        explicit array(size_t size) {
            Base::m_data = Base::m_allocator.allocate(size);
            for (size_t i = 0; i < size; i++) new(Base::m_data + i) T();
        }

        template<typename... Args>
        explicit array(Args... args) {
            Base::m_data = Base::m_allocator.allocate(sizeof...(args));
            Base::m_size = sizeof...(args);
            auto it = Base::m_data;
            ((new(it++) T(args)), ...);
        }

        array &operator=(const array &array) {
            Base::operator=(array);
            return *this;
        }
    };
}

#endif
