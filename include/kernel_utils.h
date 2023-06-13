/*******************************************************************************
* Description: c kernel utils
* Author     : wjh
* Create     : 2023/xx/xx
*
*******************************************************************************/
#ifndef __KERNEL_UTILS__
#define __KERNEL_UTILS___

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*******************************************************************************/

typedef struct list_head
{
    struct list_head *next;
    struct list_head *prev;
}list_head_t;

#define list_foreach(pos, head)                                                \
            for ( pos  = (head)->next;                                         \
                  pos != (head);                                               \
                  pos  = pos->next )

#define list_foreach_safe(pos, n, head)                                        \
            for ( pos  = (head)->next, n = pos->next;                          \
                  pos != (head);                                               \
                  pos  = n, n = pos->next)

#define list_foreach_prev(pos, head)                                           \
            for ( pos  = (head)->prev;                                         \
                  pos != (head);                                               \
                  pos  = pos->prev )

static inline void __list_init(list_head_t *head)
{
    head->prev = head;
    head->next = head;
}

static inline int __list_empty(const list_head_t *head)
{
    return (head->prev == head) ? 1 : 0;
}

// 理解为把newnode插入到prev和next中间
static inline void __list_add(list_head_t *newnode,
                              list_head_t *prev,
                              list_head_t *next)
{
    next->prev    = newnode;
    newnode->next = next;
    newnode->prev = prev;
    prev->next    = newnode;
}

// 传入需要删除节点的前一个和后一个节点
static inline void __list_del(list_head_t *prev, list_head_t *next)
{
    next->prev = prev;
    prev->next = next;
}

// 拼接
static inline void __list_splice(const list_head_t *list,
                                 list_head_t       *prev,
                                 list_head_t       *next)
{
    struct list_head *first = list->next;
    struct list_head  *last = list->prev;

    first->prev = prev;
    prev->next = first;

    last->next = next;
    next->prev = last;
}


static inline void list_init(list_head_t *head)
{
    __list_init(head);
}

static inline int list_empty(const list_head_t *head)
{
    return __list_empty(head);
}

static inline void list_add(list_head_t *newnode, list_head_t *head)
{
    __list_add(newnode, head, head->next);
}

static inline void list_add_tail(list_head_t *newnode, list_head_t *head)
{
    __list_add(newnode, head->prev, head);
}

static inline void list_del(list_head_t *entry)
{
    __list_del(entry->prev, entry->next);
    __list_init(entry);
}

// 把list接到head后面，不包括list节点，传入的list应为另一个链表的头结点
static inline void list_splice(list_head_t *list, list_head_t *head)
{
    if ( !__list_empty(list) )
    {
        __list_splice(list, head, head->next);
        __list_init(list);
    }
}

// 把list为头结点的链表插入到head链表的末尾
static inline void list_splice_tail(list_head_t *list, list_head_t *head)
{
    if ( !__list_empty(list) )
    {
        __list_splice(list, head->prev, head);
        __list_init(list);
    }
}

// 把entry节点拆出来，插入到head后
static inline void list_move_first(list_head_t *entry, list_head_t *head)
{
    if ( entry == head->next )
    {
        return;
    }

    list_del(entry);
    list_add(entry, head);
}

// 获取第一个节点
static inline void list_get_first(list_head_t **entry, list_head_t *head)
{
    if (__list_empty(head))
    {
        return;
    }
    *entry = head->next;
    list_del(head->next);
}

/*******************************************************************************
*******************************************************************************/

// 需要将第二行的typedef 改为 __typedef
#define container_of(ptr, type, member) ({\
    const __typeof( ((type *)0)->member ) *__mptr = (ptr);\
    (type *)( (void *) ( (char *)__mptr - offsetof(type,member) ) );})


/*******************************************************************************
*******************************************************************************/


#ifdef __cplusplus
}
#endif

#endif