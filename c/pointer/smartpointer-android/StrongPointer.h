/**
 * @file StrongPointer.h
 * Copyright: Android Open Source Project
 * License: Apache 2.0
 */

#ifndef ANDROID_STRONG_POINTER_H
#define ANDROID_STRONG_POINTER_H

// Location: system/core/include
// Android包装的原子操作接口（使用了GCC的原子特性）
#include <cutils/atomic.h>

// Location: bionic/libc/include
#include <stdint.h>
#include <sys/types.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------

namespace android {

// wp<>模板类前置声明
template<typename T> class wp;

// ---------------------------------------------------------------------------

// 用于sp<>的比较操作符
#define COMPARE(_op_)                                           \
inline bool operator _op_ (const sp<T>& o) const {              \
    return m_ptr _op_ o.m_ptr;                                  \
}                                                               \
inline bool operator _op_ (const T* o) const {                  \
    return m_ptr _op_ o;                                        \
}                                                               \
template<typename U>                                            \
inline bool operator _op_ (const sp<U>& o) const {              \
    return m_ptr _op_ o.m_ptr;                                  \
}                                                               \
template<typename U>                                            \
inline bool operator _op_ (const U* o) const {                  \
    return m_ptr _op_ o;                                        \
}                                                               \
inline bool operator _op_ (const wp<T>& o) const {              \
    return m_ptr _op_ o.m_ptr;                                  \
}                                                               \
template<typename U>                                            \
inline bool operator _op_ (const wp<U>& o) const {              \
    return m_ptr _op_ o.m_ptr;                                  \
}

// ---------------------------------------------------------------------------

/**
 * @class sp<T>
 */
template<typename T>
class sp
{
public:
    // 构造
    inline sp() : m_ptr(0) {}

    // （拷贝）构造
    sp(T* other);  // NOLINT(implicit)
    sp(const sp<T>& other);
    sp(sp<T>&& other);
    template<typename U> sp(U* other);  // NOLINT(implicit)
    template<typename U> sp(const sp<U>& other);  // NOLINT(implicit)
    template<typename U> sp(sp<U>&& other);  // NOLINT(implicit)

    // 析构
    ~sp();

    // 赋值操作符
    sp& operator = (T* other);
    sp& operator = (const sp<T>& other);
    sp& operator = (sp<T>&& other);
    template<typename U> sp& operator = (const sp<U>& other);
    template<typename U> sp& operator = (sp<U>&& other);
    template<typename U> sp& operator = (U* other);

    // 专门用于ProcessState
    void force_set(T* other);

    // 重置对象指针
    void clear();

    // 访问对象指针
    inline  T&      operator* () const  { return *m_ptr; }
    inline  T*      operator-> () const { return m_ptr;  }
    inline  T*      get() const         { return m_ptr; }

    // 比较操作符
    COMPARE(==)
    COMPARE(!=)
    COMPARE(>)
    COMPARE(<)
    COMPARE(<=)
    COMPARE(>=)

private:
    // 友元声明
    template<typename Y> friend class sp;
    template<typename Y> friend class wp;

    // 保存对象指针
    void set_pointer(T* ptr);
    
    // 对象指针
    T* m_ptr;
};

#undef COMPARE

// ---------------------------------------------------------------------------

// 下面是sp<>的（拷贝）构造的几种形式
// 需要注意C++11的右值引用（移动拷贝）
template<typename T>
sp<T>::sp(T* other)
    : m_ptr(other)
{
    if (other)
        other->incStrong(this);
}

template<typename T>
sp<T>::sp(const sp<T>& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr)
        m_ptr->incStrong(this);
}

template<typename T>
sp<T>::sp(sp<T>&& other)
    : m_ptr(other.m_ptr)
{
    other.m_ptr = nullptr;
}

template<typename T> template<typename U>
sp<T>::sp(U* other)
    : m_ptr(other)
{
    if (other)
        (static_cast<T*>(other))->incStrong(this);
}

template<typename T> template<typename U>
sp<T>::sp(const sp<U>& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr)
        m_ptr->incStrong(this);
}

template<typename T> template<typename U>
sp<T>::sp(sp<U>&& other)
    : m_ptr(other.m_ptr)
{
    other.m_ptr = nullptr;
}

// 析构时减少强引用但不在这里进行真正地delete对象指针m_ptr
// 当decStrong时强引用计数为1时自动delete
template<typename T>
sp<T>::~sp()
{
    if (m_ptr)
        m_ptr->decStrong(this);
}

// 下面是operator =的几种形式
// 需要注意强引用计数的更新及右值引用
template<typename T>
sp<T>& sp<T>::operator =(const sp<T>& other)
{
    T* otherPtr(other.m_ptr);
    if (otherPtr)
        otherPtr->incStrong(this);
    if (m_ptr)
        m_ptr->decStrong(this);
    m_ptr = otherPtr;
    return *this;
}

template<typename T>
sp<T>& sp<T>::operator =(sp<T>&& other)
{
    if (m_ptr)
        m_ptr->decStrong(this);
    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;
    return *this;
}

template<typename T>
sp<T>& sp<T>::operator =(T* other)
{
    if (other)
        other->incStrong(this);
    if (m_ptr)
        m_ptr->decStrong(this);
    m_ptr = other;
    return *this;
}

template<typename T> template<typename U>
sp<T>& sp<T>::operator =(const sp<U>& other)
{
    T* otherPtr(other.m_ptr);
    if (otherPtr)
        otherPtr->incStrong(this);
    if (m_ptr)
        m_ptr->decStrong(this);
    m_ptr = otherPtr;
    return *this;
}

template<typename T> template<typename U>
sp<T>& sp<T>::operator =(sp<U>&& other)
{
    if (m_ptr)
        m_ptr->decStrong(this);
    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;
    return *this;
}

template<typename T> template<typename U>
sp<T>& sp<T>::operator =(U* other)
{
    if (other)
        (static_cast<T*>(other))->incStrong(this);
    if (m_ptr)
        m_ptr->decStrong(this);
    m_ptr = other;
    return *this;
}

// 强制保存对象指针并增加强引用计数
template<typename T>
void sp<T>::force_set(T* other)
{
    other->forceIncStrong(this);
    m_ptr = other;
}

// 重置对象指针并减少强引用计数
template<typename T>
void sp<T>::clear()
{
    if (m_ptr) {
        m_ptr->decStrong(this);
        m_ptr = 0;
    }
}

// 简单地保存对象指针
template<typename T>
void sp<T>::set_pointer(T* ptr)
{
    m_ptr = ptr;
}

}; // namespace android

// ---------------------------------------------------------------------------

#endif // ANDROID_STRONG_POINTER_H
