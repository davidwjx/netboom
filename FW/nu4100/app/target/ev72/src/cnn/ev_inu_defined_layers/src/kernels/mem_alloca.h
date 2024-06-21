/*
 *
 * Usage example:
 * Initialize memory object with VCCM memory information
 * MME_ALLOCA_INIT(start_ptr, mem_size_bytes);
 * 
 * Allocate data in function body:
 * MEM_NEW[_ALIGN]  ----> T *array;
 * MEM_NEW_BOUND[_ALIGN]  ----> T *array[2][4];
 * 
 * Check if allocation ok before processing
 *  if (MEM_VALIDATE) return -1;
 */

/* 
 * File:   mem_alloca.h
 * Author: user
 *
 * Created on April 28, 2018, 5:34 PM
 */

#ifndef MEM_ALLOCA_H
#define MEM_ALLOCA_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _AllocaObj {
        uintptr_t _vccm_pos;
        uintptr_t _vccm_end;
        uintptr_t _vccm_size;
        int _valid;
    } AllocaObj;

    static inline uintptr_t value_align(uintptr_t value, uintptr_t align) {
        return ((value + align - 1) / align) * align;
    }

    static inline void mem_alloca_init(AllocaObj *info, uintptr_t vccm_start, uintptr_t vccm_size) {
        info->_vccm_pos = vccm_start;
        info->_vccm_end = vccm_start + vccm_size;
        info->_valid = (vccm_size > 0) ? 1 : 0;
        info->_vccm_size = vccm_size;
    }

    static inline void *mem_alloca(AllocaObj *info, uintptr_t size) {
        void * ptr = (void*) info->_vccm_pos;
        if (size > info->_vccm_size) {
            info->_valid = 0;
            return 0;
        }
        info->_vccm_pos += size;
        info->_vccm_size -= size;
        return ptr;
    }

    static inline void *mem_alloca_aligned(AllocaObj *info, uintptr_t size, uintptr_t align) {
        uintptr_t aligned_addr = value_align(info->_vccm_pos, align);
        uintptr_t rem_size = (aligned_addr < info->_vccm_end) ? info->_vccm_end - aligned_addr : 0;

        if (size > rem_size) {
            info->_valid = 0;
            return 0;
        }

        info->_vccm_pos = aligned_addr + size;
        info->_vccm_size = (rem_size - size);
        return (void*) aligned_addr;
    }

    static inline void mem_alloca_bundle(void **array, int len, AllocaObj *info, uintptr_t size) {
        for (int i = 0; i < len; ++i)
            array[i] = mem_alloca(info, size);
    }

    static inline void mem_alloca_aligned_bundle(void **array, int len, AllocaObj *info, uintptr_t size, uintptr_t align) {
        for (int i = 0; i < len; ++i)
            array[i] = mem_alloca_aligned(info, size, align);
    }

    static inline unsigned int mem_validate(AllocaObj *info) {
        return (info->_valid) ? 0 : -1;
    }

    static inline uintptr_t mem_remains(AllocaObj *info) {
        return info->_vccm_size;
    }

    static inline void* mem_tail(AllocaObj *info) {
        return (void*) (info->_vccm_pos);
    }

#define MEM_ALLOCA_INIT(obj_ptr, mem_start, mem_size) mem_alloca_init(obj_ptr, (uintptr_t)mem_start, mem_size)

#define MEM_ALLOCA(obj_ptr, prefix, type, len) \
    (prefix type *)mem_alloca(obj_ptr, (len) * sizeof( type ))

#define MEM_ALLOCA_ALIGN(obj_ptr, prefix, type, len, align_bytes) \
    (prefix type *)mem_alloca_aligned(obj_ptr, (len) * sizeof( type ), align_bytes)

#define MEM_ALLOCA_BUNDLE(obj_ptr, name, size) \
    mem_alloca_bundle((void**)name, sizeof(name)/sizeof(void*), obj_ptr, size);

#define MEM_ALLOCA_BUNDLE_ALIGN(obj_ptr, name, size, align_bytes) \
    mem_alloca_aligned_bundle((void**)name, sizeof(name)/sizeof(void*), obj_ptr, size, align_bytes);


#define MEM_NEW(obj_ptr, prefix, type, name, len) \
    prefix type *name = MEM_ALLOCA(obj_ptr, prefix, type, len);

#define MEM_NEW_ALIGN(obj_ptr, prefix, type, name, len, align_bytes) \
    prefix type *name = MEM_ALLOCA_ALIGN(obj_ptr, prefix, type, len, align_bytes);

#define MEM_NEW_BOUND(obj_ptr, prefix, type, name, bound, len) \
    prefix type *name bound; MEM_ALLOCA_BUNDLE(obj_ptr, name, (len) * sizeof(type));

#define MEM_NEW_BOUND_ALIGN(obj_ptr, prefix, type, name, bound, len, align_bytes) \
    prefix type *name bound; MEM_ALLOCA_BUNDLE_ALIGN(obj_ptr, name, (len) * sizeof(type), align_bytes);

#define MEM_VALIDATE(obj_ptr) mem_validate(obj_ptr)

#define MEM_REMAINS(obj_ptr) mem_remains(obj_ptr)

#define MEM_TAIL(obj_ptr) mem_tail(obj_ptr)

#ifdef __cplusplus
}
#endif

#endif /* MEM_ALLOCA_H */

