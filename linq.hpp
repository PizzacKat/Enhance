#ifndef ENHANCE_LINQ_HPP
#define ENHANCE_LINQ_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

namespace enhance::linq {
    class exception : public std::exception {
        const char *m_what;
    public:
        explicit exception(const char *what) : m_what(what) {}
        [[nodiscard]] const char * what() const noexcept override {return m_what;}
    };

    template <typename T>
    class vector : public std::vector<T> {
    public:
        using std::vector<T>::vector;
        vector(const std::vector<T> &vec) : std::vector<T>(vec){}

        template <typename R, typename Predicate>
        vector<R> select(Predicate pred){
            std::vector<R> vector;
            for (auto it = this->begin(); it < this->end(); it++) vector.push_back(pred(*it), this->end() - it);
            return vector;
        }

        template <typename R, typename Predicate>
        R aggregate(R accumulate, Predicate pred){
            for (auto it = this->begin(); it < this->end(); it++) accumulate = pred(accumulate, *it);
            return accumulate;
        }

        template <typename R, typename Predicate>
        R aggregate(Predicate pred){
            if (this->begin() == this->end()) throw exception("No elements :(");
            R accumulate = *this->begin();
            for (auto it = this->begin()+1; it < this->end(); it++) accumulate = pred(accumulate, *it);
            return accumulate;
        }

        template <typename Predicate>
        vector<T> where(Predicate pred){
            std::vector<T> vector;
            for (auto it = this->begin(); it < this->end(); it++) if (pred(*it)) vector.push_back(*it);
            return vector;
        }

        vector<T> skip(size_t elements){
            if (this->begin() + elements >= this->end()) throw exception("Can't skip, too much :(");
            return linq_vector<T>(this->begin() + elements, this->end());
        }

        vector<T> take(size_t elements){
            if (this->begin() + elements >= this->end()) throw exception("Can't take, too much :(");
            return linq_vector<T>(this->begin(), this->begin() + elements);
        }

        template <typename Predicate = bool(const T&)>
        T first(Predicate pred = [](const T &){return true;}){
            for (auto it = this->begin(); it < this->end(); it++) if (pred(*it)) return *it;
            throw exception("There was no first :(");
        }

        template <typename Predicate = bool(const T&)>
        T first_or_default(Predicate pred = [](const T &){return true;}, T def = {}){
            for (auto it = this->begin(); it < this->end(); it++) if (pred(*it)) return *it;
            return def;
        }

        template <typename Predicate = bool(const T &)>
        T last(Predicate pred = [](const T &){return true;}){
            for (auto it = this->rbegin(); it < this->rend(); it++) if (pred(*it)) return *it;
            throw exception("There was no last :(");
        }

        template <typename Predicate = bool(const T &)>
        T last_or_default(Predicate pred = [](const T &){return true;}, T def = {}){
            for (auto it = this->rbegin(); it < this->rend(); it++) if (pred(*it)) return *it;
            return def;
        }

        template <typename R, typename Predicate = R(const T &)>
        R min(Predicate pred = [](const T &a){return a;}){
            if (this->begin() == this->end()) throw exception("No elements :(");
            R min = pred(*this->begin());
            for (auto it = this->begin()+1; it < this->end(); it++) {
                R val = pred(*it);
                if (val < min) min = val;
            }
            return min;
        }

        T min(){
            if (this->begin() == this->end()) throw exception("No elements :(");
            T min = *this->begin();
            for (auto it = this->begin()+1; it < this->end(); it++) if (*it < min) min = *it;
            return min;
        }

        template <typename R, typename Predicate = R(const T &)>
        R max(Predicate pred = [](const T &a){return a;}){
            if (this->begin() == this->end()) throw exception("No elements :(");
            R max = pred(*this->begin());
            for (auto it = this->begin()+1; it < this->end(); it++) {
                R val = pred(*it);
                if (val > max) max = val;
            }
            return max;
        }

        T max(){
            if (this->begin() == this->end()) throw exception("No elements :(");
            T max = *this->begin();
            for (auto it = this->begin()+1; it < this->end(); it++) if (*it > max) max = *it;
            return max;
        }

        bool contains(T value){
            return any([&value](const T &v){return v == value;});
        }

        template <typename Predicate = bool(const T &)>
        bool any(Predicate pred = [](const T &){return true;}){
            for (auto it = this->begin(); it < this->end(); it++) if (pred(*it)) return true;
            return false;
        }

        template <typename Predicate>
        bool all(Predicate pred){
            for (auto it = this->begin(); it < this->end(); it++) if (!pred(*it)) return false;
            return true;
        }

        template <typename Predicate = bool(const T &)>
        size_t count(Predicate pred = [](const T &){return true;}){
            size_t count = 0;
            for (auto it = this->begin(); it < this->end(); it++) if (pred(*it)) count++;
            return count;
        }
    };
}

#endif
