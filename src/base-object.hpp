#ifndef __BASE_OBJECT_HPP__
#define __BASE_OBJECT_HPP__

#include "tmem.h"
#include <new>

class  Object
{
public:
    Object() {}
    virtual ~Object() {} // Which must be defined, otherwise delete will not triger destructor.

    /**
     * Overloading new/delete operators
     */
    /// normal
    void* operator new (std::size_t size) //throw(std::bad_alloc) 
        { 
            void* p = TMemAlloc(_memHandle, size);
            return p;
        }
    void  operator delete (void* p)// throw() 
        { 
            if (p) 
                TMemFree(_memHandle, p);
        }
    void* operator new (std::size_t size, const std::nothrow_t&) //throw(std::bad_alloc) 
        { 
            void* p = TMemAlloc(_memHandle, size);
            return p;
        }
    void  operator delete (void* p, std::nothrow_t&) 
        { 
            if (p)
                TMemFree(_memHandle, p);
        }

    /// array
    void* operator new[]    (std::size_t size) //throw(std::bad_alloc) 
        { 
            void* p = TMemAlloc(_memHandle, size);
            return p;
        }
    void  operator delete[] (void* p) //throw() 
        { 
            if (p) 
                TMemFree(_memHandle, p);
        }
    void* operator new[]    (std::size_t size, std::nothrow_t&) //throw(std::bad_alloc) 
        { 
            void* p = TMemAlloc(_memHandle, size);
            return p;
        }
    void  operator delete[] (void* p, std::nothrow_t&) //throw() 
        { 
            if (p) 
                TMemFree(_memHandle, p);
        }


    /**
     * Properties
     */
    static void    setMemHandle(THandle memHandle) { _memHandle = memHandle; }
    static THandle memHandle() { return _memHandle; }

private:
    static THandle  _memHandle;
};


#endif
