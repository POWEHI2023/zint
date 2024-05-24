#ifndef __ZLINK_LIST__
#define __ZLINK_LIST__

#include "memory_control.h"

typedef __INT64_TYPE__ pointer;

typedef struct link_list
{
    pointer  front;
    pointer  next;
} link_list;

#define offset_of(_type, _member) (size_t)((char*)(&((_type*)0)->_member))

#define content_of(_ptr, _type, _member) \
({  \
    typeof( ((_type*)0)->_member )* __mptr = (_ptr);  \
    (_type*)( (char*)__mptr - offset_of(_type, _member) );   \
})

#define link_lisk_next(l) ((link_list*)((l)->next))

#define link_list_front(l) ((link_list*)((l)->front))

#define link_list_insert(l, _link) \
do {    \
    if (l == 0)     \
    {               \
        break;      \
    }               \
    link_list* _next = link_lisk_next(l);   \
    if (_next != 0)                         \
    {                                       \
        (_link)->next = (pointer)_next;       \
        _next->front = (pointer)(_link);      \
    }                                       \
    (l)->next = (pointer)(_link);               \
    (_link)->front = (pointer)(l);            \
} while (0);

#define link_list_remove(l) \
({  \
    if ((l)->front == 0)                      \
    {                                       \
        link_list* _ret = (l);                \
        (l) = (link_list*)((l)->next);            \
        (l)->front = 0;                       \
        return ret;                         \
    }                                       \
    link_list* _front = link_list_front(l); \
    _front->next = (pointer)((l)->next);        \
    if ((l)->next != 0)                       \
        ((link_list*)((l)->next))->front = (pointer)_front;   \
    (l)->next = (l)->front = 0;                 \
    l;                               \
})

#define link_list_reset(_mem) (((link_list*)_mem)->next = ((link_list*)_mem)->front = 0)

#define link_list_make() \
({  \
    void* _mem = melloc(sizeof(link_list)); \
    link_list_reset(_mem);  \
    (pointer)_mem;          \
})

#define link_list_free(_mem) (free((link_list*)_mem))

#endif