#ifndef KMALLOC_H
#define KMALLOC_H

#include <mem/vaddr.h>
#include <crt/header.h>
#include <stddef.h>

EXTERN_C_BEGIN

// Initialize valloc and vfree
void    vinit(void);
// Allocate virtual page
vaddr_t valloc(void);
// Free virtual page
void    vfree(vaddr_t vaddr);
// Allocate block
void*   kmalloc(size_t size);
// Deallocate block
void    kfree(void* ptr);

#ifdef __cplusplus
inline void* operator new( size_t count ) { return kmalloc(count); }
inline void  operator delete( void* ptr )      noexcept { kfree(ptr); }
inline void *operator new(size_t, void *p)     throw() { return p; }
inline void *operator new[](size_t, void *p)   throw() { return p; }
inline void  operator delete  (void *, void *) noexcept { };
inline void  operator delete[](void *, void *) noexcept { };
#endif

EXTERN_C_END

#endif
