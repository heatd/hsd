/* SPDX-License-Identifier: GPL-2.0-ONLY */
#ifndef _LIST_H
#define _LIST_H

/* Implementation of struct list_head like linux, so circular */
struct list_head
{
    struct list_head *prev, *next;
};


#define LIST_HEAD_INIT(lh) \
    {                      \
        &(lh), &(lh)       \
    }

#define DEFINE_LIST(name) struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *lh)
{
    lh->prev = lh;
    lh->next = lh;
}

/* Okay, this was very clearly inspired by linux's list.h but it just links together the
 * new node with rest of the list
 */
static inline void __list_add(struct list_head *_new, struct list_head *prev,
                              struct list_head *next)
{
    next->prev = _new;
    _new->next = next;
    _new->prev = prev;
    prev->next = _new;
}

static inline void list_add(struct list_head *_new, struct list_head *head)
{
    __list_add(_new, head, head->next);
}

static inline void list_add_tail(struct list_head *_new, struct list_head *head)
{
    __list_add(_new, head->prev, head);
}


static inline void list_remove_bulk(struct list_head *prev, struct list_head *next)
{
    prev->next = next;
    next->prev = prev;
}

static inline void list_remove(struct list_head *node)
{
    list_remove_bulk(node->prev, node->next);
}

static inline int list_is_empty(const struct list_head *head)
{
    return head->next == head;
}

void list_assert_correct(struct list_head *head);

static inline struct list_head *list_first_element(struct list_head *head)
{
    if (list_is_empty(head))
        return 0;
    return head->next;
}

static inline struct list_head *list_last_element(struct list_head *head)
{
    if (list_is_empty(head))
        return 0;
    return head->prev;
}

static inline void list_reset(struct list_head *head)
{
    INIT_LIST_HEAD(head);
}

#define list_for_every(lh) for (struct list_head *l = (lh)->next; l != (lh); l = l->next)

/* Again, this one is also very clearly inspired by linux */
#define list_for_every_safe(lh)                                           \
    for (struct list_head *l = (lh)->next, *____tmp = l->next; l != (lh); \
         l = ____tmp, ____tmp = l->next)

#define offsetof(type, member) __builtin_offsetof(type, member)
#define container_of(ptr, type, member) ((type *) ((char *) ptr - offsetof(type, member)))

#endif
