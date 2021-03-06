// allocator with secure-zero wipe 
// at object desctrution time
//
// usage: use it just like a standard 
// allocator (it is stateless)
//
// tested with non-standard containers
// such as boost property tree
//
// depends on OpenSSL:
// link with libcrypto.so (-lcrypto)

#pragma once

#include <cstddef>
#include <memory>
#include <limits>
#include <openssl/crypto.h>

#ifdef _WIN32
#define allocator_api _DECLSPEC_ALLOCATOR
#else
#define allocator_api
#endif

template <typename T>
struct secure_allocator
{
    template<typename U>
    struct rebind
    {
        typedef secure_allocator<U>
            other;
    };

    typedef T
        value_type;

    typedef value_type*
        pointer;

    typedef const value_type*
        const_pointer;

    typedef value_type&
        reference;

    typedef const value_type&
        const_reference;

    typedef std::size_t
        size_type;

    typedef std::ptrdiff_t
        difference_type;

    secure_allocator() {}

    secure_allocator(const 
        secure_allocator<T>&) {}

    template<class U>
        secure_allocator(const 
        secure_allocator<U>&) {}

    template<class U>
        secure_allocator<T>& operator=(
        const secure_allocator<U>&)
    {
        return (*this);
    }
    
    auto address (reference _Ref) 
        const ->pointer 
    {
        return &_Ref;
    }

    auto address (const_reference _Ref) 
        const ->const_pointer 
    {
        return &_Ref;
    }

    allocator_api auto allocate (
        size_type _N, const void* _H=0)
        ->pointer
    {
        if (_N > max_size())
            throw std::bad_alloc();

        return static_cast<pointer>(
            ::operator new (_N*
                sizeof (value_type)));
    }

    void deallocate(pointer _Ptr, 
        size_type _N)
    {
        ::OPENSSL_cleanse(_Ptr, 
            _N*sizeof(value_type)); 
        
        ::operator delete(_Ptr); 
    }

    auto max_size() const 
        ->size_type
    {
        return std::numeric_limits
            <size_type>::max()/
            sizeof (value_type);
    } 

    void construct (pointer _Ptr, 
        const T& _Val)
    {
        new (static_cast<T*>(
            _Ptr)) T (_Val);
    }

    void destroy(pointer _Ptr)
    {
        static_cast<T*>(
            _Ptr)->~T();
    }

    #if __cpluplus >= 201103L
    template<typename U, 
        typename... Args>
    void construct (U* _Ptr, 
        Args&&  ... _Args) 
    {
        ::new (static_cast<void*> 
        (_Ptr)) U (std::forward<
            Args>(_Args)...);
    }

    template<typename U>
    void destroy(U* _Ptr) 
    {
        _Ptr->~U();
    }
    #endif
};

typedef std::basic_string<char, 
    std::char_traits<char>, 
        secure_allocator<char>
            > secure_char_string;


namespace std
{// extend standard with comparison 
    // for secure_allocator(s)
    template<class T, class U>
    bool operator == (
        const secure_allocator<T>& _Lhs,
        const secure_allocator<U>& _Rhs)
    {// stateless allocators, 
        // so they are always equal
        return true;
    }

    template<class T, class U>
    bool operator != (
        const secure_allocator<T>& _Lhs,
        const secure_allocator<U>& _Rhs)
    {// stateless allocator, 
        // so they are never unequal
        return false;
    }
}
