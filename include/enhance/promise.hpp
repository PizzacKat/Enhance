#ifndef ENHANCE_PROMISE_HPP
#define ENHANCE_PROMISE_HPP

#include <thread>
#include <functional>
#include <memory>
#include <any>
#include <mutex>

namespace enhance {
    enum promise_status {
        PENDING, RESOLVED, REJECTED
    };

    template <typename T, typename Err = std::exception_ptr>
    class promise {
    public:
        using Resolve = std::function<void(const T &)>;
        using Reject = std::function<void(const Err &)>;
        using Then = std::function<void(const T &)>;
        using Except = std::function<void(const Err &)>;
        using Finally = std::function<void()>;
        using Executor = std::function<void(Resolve, Reject)>;
        promise()= default;

        promise(const Executor &executor){
            m_thread = std::make_shared<std::thread>(executor, resolve_ev(), reject_ev());
        }

        ~promise(){
            if (m_thread->joinable())
                m_thread->detach();
        }

        T await_result(){
            if (*m_status == PENDING) {
                while (*m_status == PENDING) std::this_thread::sleep_for(std::chrono::nanoseconds(600));
            }
            if (*m_status == REJECTED) {
                if (m_except.empty()) {
                    if constexpr (std::is_same_v<std::exception_ptr, Err>)
                    std::rethrow_exception(*m_exception);
                    else
                    throw *m_exception;
                }
                else {
                    for (const Except &except: m_except)
                        except(*m_exception);
                    for (const Finally &finally: m_finally)
                        finally();
                    return {};
                }
            }
            for (const Then &then : m_then)
                then(*m_value);
            for (const Finally &finally: m_finally)
                finally();
            return *m_value;
        }

        [[nodiscard]] bool is_pending() const{
            return *m_status == PENDING;
        }

        T operator*(){
            return await_result();
        }

        promise &then(const Then &callback){
            m_then.push_back(callback);
            return *this;
        }

        promise &except(const Except &callback){
            m_except.push_back(callback);
            return *this;
        }

        promise &finally(const Finally &callback){
            m_finally.push_back(callback);
            return *this;
        }

        static promise<T> resolve(T value){
            promise<T> promise;
            *promise.m_value = value;
            *promise.m_status = RESOLVED;
            return promise;
        }

        static promise<T> reject(const Err &e){
            promise<T> promise;
            *promise.m_exception = e;
            *promise.m_status = REJECTED;
            return promise;
        }

        template <typename... Args>
        static promise<std::vector<T>> all(Args... args){
            std::vector<promise<T>> promises;
            ((promises.push_back(args)), ...);
            return promise<std::vector<T>>([promises](auto resolve, auto reject) {
                std::vector<promise<T>> pms = promises;
                std::vector<T> values;
                for (promise<T> &promise: pms) {
                    values.push_back(*promise.except([&reject](const Err &e) { reject(e); }));
                }
                resolve(values);
            });
        }

        template <typename... Args>
        static promise<T, std::vector<Err>> any(Args... args){
            std::vector<promise<T>> promises;
            ((promises.push_back(args)), ...);
            return promise<T, std::vector<Err>>([promises](auto resolve, auto reject) {
                std::vector<promise<T>> pms = promises;
                std::vector<Err> exceptions;
                for (promise<T> &promise: pms) {
                    *promise.except([&exceptions](const Err &e){exceptions.push_back(e);}).then([&resolve](T value){resolve(value);});
                }
                reject(exceptions);
            });
        }
    private:
        Resolve resolve_ev(){
            std::shared_ptr<T> value = m_value;
            std::shared_ptr<promise_status> status = m_status;
            return [status, value](T v) {
                if (*status == PENDING) {
                    *value = v;
                    *status = RESOLVED;
                }
            };
        }

        Reject reject_ev(){
            std::shared_ptr<Err> exception = m_exception;
            std::shared_ptr<promise_status> status = m_status;
            return [status, exception](const Err &e) {
                if (*status == PENDING) {
                    *exception = e;
                    *status = REJECTED;
                }
            };
        }

        std::shared_ptr<T> m_value = std::make_shared<T>();
        std::shared_ptr<Err> m_exception = std::make_shared<Err>();
        std::shared_ptr<promise_status> m_status = std::make_shared<promise_status>(PENDING);
        std::shared_ptr<std::thread> m_thread = nullptr;
        std::vector<Then> m_then;
        std::vector<Except> m_except;
        std::vector<Finally> m_finally;
    };

    template <typename Err>
    class promise<void, Err> {
    public:
        using Resolve = std::function<void()>;
        using Reject = std::function<void(const Err &)>;
        using Then = std::function<void()>;
        using Except = std::function<void(const Err &)>;
        using Finally = std::function<void()>;
        using Executor = std::function<void(Resolve, Reject)>;
        promise()= default;

        promise(const Executor &executor){
            m_thread = std::make_shared<std::thread>(executor, resolve_ev(), reject_ev());
        }

        ~promise(){
            if (m_thread->joinable())
                m_thread->detach();
        }

        void await_result(){
            if (*m_status == PENDING) {
                while (*m_status == PENDING) std::this_thread::sleep_for(std::chrono::nanoseconds(600));
            }
            if (*m_status == REJECTED) {
                if (m_except.empty()) {
                    if constexpr (std::is_same_v<std::exception_ptr, Err>)
                    std::rethrow_exception(*m_exception);
                    else
                    throw *m_exception;
                }
                else {
                    for (const Except &except: m_except)
                        except(*m_exception);
                    for (const Finally &finally: m_finally)
                        finally();
                    return;
                }
            }
            for (const Then &then : m_then)
                then();
            for (const Finally &finally: m_finally)
                finally();
        }

        [[nodiscard]] bool is_pending() const{
            return *m_status == PENDING;
        }

        void operator*(){
            return await_result();
        }

        promise &then(const Then &callback){
            m_then.push_back(callback);
            return *this;
        }

        promise &except(const Except &callback){
            m_except.push_back(callback);
            return *this;
        }

        promise &finally(const Finally &callback){
            m_finally.push_back(callback);
            return *this;
        }

        static promise<void> resolve(){
            promise<void> promise;
            *promise.m_status = RESOLVED;
            return promise;
        }

        static promise<void> reject(const Err &e){
            promise<void> promise;
            *promise.m_exception = e;
            *promise.m_status = REJECTED;
            return promise;
        }

        template <typename... Args>
        static promise<void> all(Args... args){
            std::vector<promise<void>> promises;
            ((promises.push_back(args)), ...);
            return promise<void>([promises](auto resolve, auto reject) {
                std::vector<promise<void>> pms = promises;
                for (promise<void> &promise: pms) {
                    *promise.except([&reject](const Err &e) { reject(e); });
                }
                resolve();
            });
        }

        template <typename... Args>
        static promise<void, std::vector<Err>> any(Args... args){
            std::vector<promise<void>> promises;
            ((promises.push_back(args)), ...);
            return promise<void, std::vector<Err>>([promises](auto resolve, auto reject) {
                std::vector<promise<void>> pms = promises;
                std::vector<Err> exceptions;
                for (promise<void> &promise: pms) {
                    *promise.except([&exceptions](const Err &e){exceptions.push_back(e);}).then([&resolve](){resolve();});
                }
                reject(exceptions);
            });
        }
    private:
        Resolve resolve_ev(){
            std::shared_ptr<promise_status> status = m_status;
            return [status]() {
                if (*status == PENDING)
                    *status = RESOLVED;
            };
        }

        Reject reject_ev(){
            std::shared_ptr<Err> exception = m_exception;
            std::shared_ptr<promise_status> status = m_status;
            return [exception, status](const Err &e) {
                if (*status == PENDING) {
                    *exception = e;
                    *status = REJECTED;
                }
            };
        }

        std::shared_ptr<Err> m_exception = std::make_shared<Err>();
        std::shared_ptr<promise_status> m_status = std::make_shared<promise_status>(PENDING);
        std::shared_ptr<std::thread> m_thread = nullptr;
        std::vector<Then> m_then;
        std::vector<Except> m_except;
        std::vector<Finally> m_finally;
    };
}

#endif //ENHANCE_PROMISE_HPP
