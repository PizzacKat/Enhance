#ifndef ENHANCE_OBJECT_HPP
#define ENHANCE_OBJECT_HPP

class object {
public:
    object()= default;

    object(std::nullptr_t) : object(){

    }

    template <typename T>
    static object cast(T &ref){
        object obj;
        obj.m_data = &ref;
        obj.m_type = typeid(T);
        obj.m_size = sizeof(T);
        obj.m_copy = [](void *ptr) -> void *{ return ptr; };
        obj.m_free = [](void *) -> void{};
        return obj;
    }

    template <typename T>
    static object copy(const T &value){
        object obj;
        obj.m_data = new T(value);
        obj.m_type = typeid(const T &);
        obj.m_size = sizeof(T);
        obj.m_copy = [](void *ptr) -> void *{ return new T(*(T *)ptr); };
        obj.m_free = [](void *ptr) -> void{ delete (T *)ptr; };
        return obj;
    }

    object(const object &object){
        m_data = object.m_copy(object.m_data);
        m_type = object.m_type;
        m_size = object.m_size;
        m_copy = object.m_copy;
        m_free = object.m_free;
    }

    template <typename T>
    object(const T &value){
        m_data = new T(value);
        m_type = typeid(const T &);
        m_size = sizeof(T);
        m_copy = [](void *ptr) -> void *{ return new T(*(T *)ptr); };
        m_free = [](void *ptr) -> void{ delete (T *)ptr; };
    }

    template <typename T>
    object(T &ref){
        m_data = &ref;
        m_type = typeid(T);
        m_size = sizeof(T);
        m_copy = [](void *ptr) -> void *{ return ptr; };
        m_free = [](void *) -> void{};
    }

    object &operator=(const object &object){
        if (&object == this) return *this;
        m_free(m_data);
        m_data = object.m_copy(object.m_data);
        m_type = object.m_type;
        m_size = object.m_size;
        m_copy = object.m_copy;
        m_free = object.m_free;
    }

    template <typename T>
    explicit operator T &() const{
        if (m_type != typeid(std::remove_cv_t<T>))
            throw std::bad_cast("Cannot convert from type_builder " + demangle(m_type.name()) + " to type_builder " + demangle(typeid(std::remove_cv_t<T>).name()));
        return *static_cast<T *>(m_data);
    }

    template <typename T>
    T &get() const{
        return *(T *)m_data;
    }

    void *data() {
        return m_data;
    }

    [[nodiscard]] const void *data() const{
        return m_data;
    }

    [[nodiscard]] size_t size() const{
        return m_size;
    }

    [[nodiscard]] const std::type_index &type() const{
        return m_type;
    }

    template <typename T>
    [[nodiscard]] bool is() const{
        return is(typeid(T));
    }

    [[nodiscard]] bool is(const std::type_info &type) const{
        return m_type == type;
    }

    ~object(){
        m_free(m_data);
    }
private:
    void *m_data = nullptr;
    size_t m_size = 0;
    std::type_index m_type = typeid(void);
    void *(*m_copy)(void *) = [](void *) -> void *{ return nullptr; };
    void (*m_free)(void *) = [](void *) -> void{};
};

#endif //ENHANCE_OBJECT_HPP
