#pragma once

#include <memory>
#include <mutex>
#include <iostream>
template <typename T>
class Singleton {
protected:
    Singleton() = default;
    static std::shared_ptr<T> instance_;
public:
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>& st) = delete;
    Singleton(const Singleton<T>&&) = delete;
    Singleton& operator=(const Singleton<T>&& st) = delete;
    static std::shared_ptr<T> GetInstance() {
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]() {
        	instance_ = std::shared_ptr<T>(new T);
            });

        return instance_;
    }
    void PrintAddress() {
        std::cout << instance_.get() << std::endl;
    }
    ~Singleton() {
        std::cout << "this is singleton destruct" << '\n';
    }
};

template <typename T>
std::shared_ptr<T> Singleton<T>::instance_ = nullptr;