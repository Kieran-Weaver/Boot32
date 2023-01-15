#ifndef BOOT32_ATOMIC_H
#define BOOT32_ATOMIC_H

#include <stdbool.h>

/* stdatomic.h implemented with GCC builtins */

#define _Atomic
#define memory_order_relaxed __ATOMIC_RELAXED
#define memory_order_consume __ATOMIC_CONSUME
#define memory_order_acquire __ATOMIC_ACQUIRE
#define memory_order_release __ATOMIC_ACQUIRE
#define memory_order_acq_rel __ATOMIC_ACQ_REL
#define memory_order_seq_cst __ATOMIC_SEQ_CST

#define atomic_store(obj, val) atomic_store_explicit(obj, val, memory_order_seq_cst)
#define atomic_load(obj) atomic_load_explicit(obj, memory_order_seq_cst)
#define atomic_exchange(obj, val) atomic_exchange_explicit(obj, val, memory_order_seq_cst)
#define atomic_compare_exchange_strong(obj, expected, val) atomic_compare_exchange_strong_explicit(obj, expected, val, memory_order_seq_cst, memory_order_seq_cst)
#define atomic_compare_exchange_weak(obj, expected, val) atomic_compare_exchange_strong_explicit(obj, expected, val, memory_order_seq_cst, memory_order_seq_cst)
#define atomic_fetch_add(obj, val) atomic_fetch_add_explicit(obj, val, memory_order_seq_cst)
#define atomic_fetch_sub(obj, val) atomic_fetch_sub_explicit(obj, val, memory_order_seq_cst)
#define atomic_fetch_or(obj, val)  atomic_fetch_or_explicit(obj, val, memory_order_seq_cst)
#define atomic_fetch_xor(obj, val) atomic_fetch_xor_explicit(obj, val, memory_order_seq_cst)
#define atomic_fetch_and(obj, val) atomic_fetch_and_explicit(obj, val, memory_order_seq_cst)

#define atomic_store_explicit(obj, val, memorder) __atomic_store_n(obj, val, memorder)
#define atomic_load_explicit(obj, memorder) __atomic_load_n(obj, memorder)
#define atomic_exchange_explicit(obj, val, memorder) __atomic_exchange_n(obj, val, memorder)
#define atomic_compare_exchange_strong_explicit(obj, expected, val, succ, fail) __atomic_compare_exchange_n(obj, expected, val, false, succ, fail)
#define atomic_compare_exchange_weak_explicit(obj, expected, val, succ, fail) __atomic_compare_exchange_n(obj, expected, val, true, succ, fail)
#define atomic_fetch_add_explicit(obj, val, memorder) __atomic_fetch_add(obj, val, memorder)
#define atomic_fetch_sub_explicit(obj, val, memorder) __atomic_fetch_add(obj, val, memorder)
#define atomic_fetch_or_explicit(obj, val, memorder) __atomic_fetch_add(obj, val, memorder)
#define atomic_fetch_xor_explicit(obj, val, memorder) __atomic_fetch_add(obj, val, memorder)
#define atomic_fetch_and_explicit(obj, val, memorder) __atomic_fetch_add(obj, val, memorder)

#endif
