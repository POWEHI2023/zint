#ifndef __ZMEMORY_CONTROL__
#define __ZMEMORY_CONTROL__

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "link_list.h"
#include "basic_struct.h"

#include "output.h"

#define __DEBUG

#define __BACK_OFFSET 8;
typedef __INT64_TYPE__ __BACK_TYPE;


void* make_private_memory(size_t size)
{
    const long PAGE_SIZE = sysconf(_SC_PAGESIZE);

    if (size < PAGE_SIZE)
    {
        printf("SUGGEST: size is too small [%ld : %ld]\n", size, PAGE_SIZE);
    }

    size += __BACK_OFFSET;

    if (size % PAGE_SIZE != 0)
    {
        size += PAGE_SIZE - (size % PAGE_SIZE);
    }

    #ifdef __DEBUG
    printf("Total memory size: %ld\n", size);
    #endif

    void* _mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (_mem == MAP_FAILED)
    {
        fprintf(stderr, "Make private memory failed.\n");
        return 0;
    }
    
    #ifdef __DEBUG
    printf("Original ptr: %ld\n", (size_t)_mem);
    #endif

    *(__BACK_TYPE*)(_mem) = size;
    return _mem + __BACK_OFFSET;
}

int unmap_private_memory(void* _mem)
{
    _mem -= __BACK_OFFSET;
    
    __BACK_TYPE* size = (__BACK_TYPE*)_mem;

    #ifdef __DEBUG
    printf("Unmap size: %ld\n", *size);
    #endif

    int ret = munmap(_mem, *size);
    if (ret == -1)
    {
        fprintf(stderr, "Unmap private memory failed,\n");
        return -1;
    }

    #ifdef __DEBUG
    printf("Unmap successfully.\n");
    #endif

    return 0;
}

typedef struct file_mapper
{
    char const* file_name;

    size_t      total_size;
    size_t      mapped_size;
    size_t      mapped_offset;

    void*       ptr;
} file_mapper;

// shared map a file
file_mapper* make_file_memory(char const* file_name, size_t offset, size_t length)
{
    int fd = open(file_name, O_RDWR);

    if (fd < 0)
    {
        fprintf(stderr, "Opening file[%s] error.\n", file_name);
        return 0;
    }

    void* _mem = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
    if (_mem == MAP_FAILED)
    {
        fprintf(stderr, "Map file[%s] error.\n", file_name);
        return 0;
    }

    file_mapper* _fm = (file_mapper*)malloc(sizeof(file_mapper));
    _fm->file_name = file_name;
    _fm->total_size = fseek(fdopen(fd, "r+"), 0, SEEK_END);
    _fm->mapped_size = length;
    _fm->mapped_offset = offset;
    _fm->ptr = _mem;

    close(fd);

    return _fm;
}

int unmap_file_memory(file_mapper* fm)
{
    int ret = munmap(fm->ptr, fm->mapped_size);
    if (ret == -1)
    {
        fprintf(stderr, "Unmap file[%s] error.\n", fm->file_name);
        return -1;
    }

    free(fm);
    return 0;
}

// allocate zint and zelem in this zpage
// zint allocated from top to bottom
// zelem allocated from bottom to top
// zpage is full when min(sizeof_zin, sizeof_zelem) less than (zelem_brk - zint_brk - 1). 
// two byte for brk is not used
typedef struct zpage
{
    size_t zint_brk;
    size_t zelem_brk;

    void* __mem;
    link_list list;
} zpage;

// construct a zpage, if __ptr is not NULL, move __ptr front to next position
// you should make sure the memory of __ptr has enough space for a zpage
#define zpage_make(__ptr) \
({  \
    size_t PAGE_SIZE = sysconf(_SC_PAGESIZE);\
    zpage* __page = 0;\
    if (__ptr == 0)  \
    {   \
        __page = (zpage*)malloc(sizeof(zpage) + PAGE_SIZE);\
    }   \
    else    \
    {   \
        __page = (zpage*)__ptr; \
        __ptr += sizeof(zpage) + PAGE_SIZE; \
    }   \
    __page->__mem = (void*)((char*)__page + sizeof(zpage)); \
    __page->zint_brk = (size_t)(__page->__mem); \
    __page->zelem_brk = __page->zint_brk + PAGE_SIZE;   \
    link_list_reset(&__page->list);  \
    __page; \
})

// collect memory and back the memory to OS
// make sure __ptr is NULL when making zpage by zpage_make(__ptr)
#define zpage_free(__ptr) (free(__ptr))

#define zpage_next(__page) \
({  \
    zpage* __ret = 0;   \
    link_list* __link = link_lisk_next(&(__page->list));\
    if (__link != 0)\
    {   \
        __ret = content_of(__link, zpage, list);\
    }   \
    __ret;  \
})

#define zpage_front(__page) \
({  \
    zpage* __ret = 0;   \
    link_list* __link = link_list_front(&(__page->list));\
    if (__link != 0)    \
    {   \
        __ret = content_of(__link, zpage, list);\
    }   \
    __ret;  \
})



// allocate for zint, zelem
typedef struct zcache
{
    zpage* __head;

    zpage* __busy;
    zpage* __full;

} zcache;

zcache* super_cache = 0;

// make sure you would not init multiple times
int zcache_init(void)
{
    if (super_cache == 0)
    {
        super_cache = (zcache*)malloc(sizeof(zcache));
    }

    super_cache->__busy = 0;
    super_cache->__full = 0;

    size_t __null_ptr = 0;
    if (super_cache->__head == 0)
    {
        super_cache->__head = (zpage*)malloc(sizeof(zpage));
        // super_cache->__head->list = (link_list*)((char*)(super_cache->__head) + sizeof(zpage));
    }

    link_list_reset(&(super_cache->__head->list));
    super_cache->__head->__mem = 0;
    super_cache->__head->zelem_brk = 0;
    super_cache->__head->zint_brk = 0;

    zpage* page = zpage_make(__null_ptr);
    link_list_insert(&(super_cache->__head->list), &(page->list));

    return 0;
}

#define ZCACHE_FREE_SUCCESS 0x00
#define ZCACHE_FREE_FAILED 0x10
#define ZCACHE_FREE_OCUPIED 0x20

int zcache_free(void)
{
    if (super_cache->__busy != 0 || super_cache->__full != 0)
    {
        return ZCACHE_FREE_OCUPIED;
    }

    zpage* __page = super_cache->__head;
    zpage* __next = zpage_next(__page);

    while (__next != 0)
    {
        __page = __next;
        __next = zpage_next(__page);

        zpage_free(__page);
    }

    free(super_cache->__head);
    free(super_cache);

    return ZCACHE_FREE_SUCCESS;
}



zint* __zpage_zint_allocate(zpage* __page);

zelem* __zpage_zelem_allocate(zpage* __page);

zint* __zcache_zint_allocate(void);

zelem* __zcache_zelem_allocate(void);

zint* zint_allocate(void);

zelem* zelem_allocate(void);

int zelem_allocate_free(zelem* __elem);

int zint_allocate_free(zint* __x);



int zpage_display(zpage* __page)
{
    printf("[%ld] display zpage data:\n", (size_t)__page);
    printf("\t -> mem: %ld, \n\t -> list: %ld, \n", (size_t)(__page->__mem), (size_t)(&__page->list));
    printf("\t    [front: %ld, next: %ld] \n", (size_t)(__page->list.front), (size_t)(__page->list.next));
    printf("\t -> zint_brk: %ld, \n\t -> zelem_brk: %ld, \n", (size_t)(__page->zint_brk), (size_t)(__page->zelem_brk));
}

int zcache_display(void)
{
    printf("\n============================== \
        \nzcache display[metadata]:\n");

    zpage* __page = super_cache->__head;

    int count = 0;
    __page = zpage_next(__page);
    while (__page != 0)
    {
        count += 1;
        __page = zpage_next(__page);
    }
    printf("\tzcache free page: %d\n", count);

    count = 0;
    __page = super_cache->__busy;
    while (__page != 0)
    {
        count += 1;
        __page = zpage_next(__page);
    }
    printf("\tzcache busy page: %d\n", count);

    count = 0;
    __page = super_cache->__full;
    while (__page != 0)
    {
        count += 1;
        __page = zpage_next(__page);
    }
    printf("\tzcache full page: %d\n", count);

    // display super_cache->head
    printf("\n");
    __page = super_cache->__head;
    zpage_display(super_cache->__head);
    printf("\n");

    __page = zpage_next(__page);
    while (__page != 0)
    {
        zpage_display(__page);
        printf("\n");
        __page = zpage_next(__page);
    }
}

int zint_display(zint* __x)
{
    printf("zint[%ld]: sign: %ld, capacity: %ld, used: %ld\n", \
        (size_t)__x, (size_t)(__x->sign), (size_t)(__x->capacity), (size_t)(__x->used));

    printf("-> zint::zelem: (head)[value: %ld, [front: %ld, next: %ld]] \n", \
        (size_t)(__x->head.val), (size_t)(__x->head.list.front), (size_t)(__x->head.list.next));

    zelem* __elem = zelem_next(&(__x->head));
    while (__elem != 0)
    {
        printf("\t (...)[value: %ld, [front: %ld, next: %ld]] \n", \
            (size_t)(__elem->val), (size_t)(__elem->list.front), (size_t)(__elem->list.next));
        __elem = zelem_next(__elem);
    }
    printf("\n");

    return 0;
}


#endif