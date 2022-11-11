#ifndef ENHANCE_GENERATOR_HPP
#define ENHANCE_GENERATOR_HPP

namespace enhance::generators {
    template <typename T>
    class generator {
    protected:
        generator() = default;
        ~generator() = default;
        virtual T next(){ return {}; };
        virtual bool hasNext() { return false; };
    };
}

#endif //ENHANCE_GENERATOR_HPP
