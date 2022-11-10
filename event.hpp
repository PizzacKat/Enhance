#ifndef ENHANCE_EVENT_HPP
#define ENHANCE_EVENT_HPP

#include <vector>
#include <functional>

namespace enhance {
    template <typename Signature, typename Function = std::function<Signature>>
    class event;

    template <typename Ret, typename... Args, typename Function>
    class event<Ret(Args...), Function>{
        std::vector<Function> functions;
    public:
        event &operator +=(const Function &fnc){
            functions.push_back(fnc);
            return *this;
        }

        event &operator -=(const Function &fnc){
            auto it = std::find(functions.begin(), functions.end(), fnc);
            if (it != functions.end()) functions.erase(it);
            return *this;
        }

        Ret invoke(Args... args){
            Ret n = {};
            for (const Function &fnc : functions) n = fnc(args...);
            return n;
        }

        std::vector<Ret> invoke_all(Args... args){
            std::vector<Ret> n = {};
            for (const Function &fnc : functions) n.push_back(fnc(args...));
            return n;
        }

        Ret operator()(Args... args){
            return invoke(args...);
        }
    };

    template <typename... Args, typename Function>
    class event<void(Args...), Function>{
        std::vector<Function> functions;
    public:
        event &operator +=(const Function &fnc){
            functions.push_back(fnc);
            return *this;
        }

        event &operator -=(const Function &fnc){
            auto it = std::find(functions.begin(), functions.end(), fnc);
            if (it != functions.end()) functions.erase(it);
            return *this;
        }

        void invoke(Args... args){
            for (const Function &fnc : functions) fnc(args...);
        }

        void operator()(Args... args){
            invoke(args...);
        }
    };
}

#endif //ENHANCE_EVENT_HPP
