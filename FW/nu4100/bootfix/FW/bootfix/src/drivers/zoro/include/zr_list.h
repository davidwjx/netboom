/**
	@file   zr_list.h

	@brief  Simple doubly linked list implementation.
	        Idea is taken from Linux

	@date 2011

	@author Uri Shkolnik, Zoro Solutions Ltd

	<b> Copyright (c) 2010-2013 Zoro Solutions Ltd. </b>\n
	43 Hamelacha street, P.O. Box 8786, Poleg Industrial Park, Netanaya, ZIP 42505 Israel\n
	All rights reserved\n\n
	Proprietary rights of Zoro Solutions Ltd are involved in the
	subject matter of this material. All manufacturing, reproduction,
	use, and sales rights pertaining to this subject matter are governed
	by the license agreement. The recipient of this software implicitly
	accepts the terms of the license. This source code is the unpublished
	property and trade secret of Zoro Solutions Ltd.
	It is to be utilized solely under license from Zoro Solutions Ltd and it
	is to be maintained on a confidential basis for internal company use
	only. It is to be protected from disclosure to unauthorized parties,
	both within the Licensee company and outside, in a manner not less stringent
	than that utilized for Licensee's own proprietary internal information.
	No copies of the source or object code are to leave the premises of
	Licensee's business except in strict accordance with the license
	agreement signed by Licensee with Zoro Solutions Ltd.\n\n

	For more details - http://zoro-sw.com
	email: info@zoro-sw.com
*/

#ifndef _LIST_H
#define _LIST_H

#ifdef __cplusplus
extern "C"
{
#endif

//! Illegal address 1
#define LIST_POISON1    0xFFFFFFFF
//! Illegal address 2
#define LIST_POISON2    LIST_POISON1


struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static ZR_INLINE void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

/**
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
#ifndef CONFIG_DEBUG_LIST
static ZR_INLINE void __list_add(struct list_head *new,
                                 struct list_head *prev,
                                 struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}
#else
extern void __list_add(struct list_head *new,
                       struct list_head *prev,
                       struct list_head *next);
#endif

/**
 * list_add - add a new entry
 * @param new: new entry to be added
 * @param head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static ZR_INLINE void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}


/**
 * list_add_tail - add a new entry
 * @param new: new entry to be added
 * @param head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static ZR_INLINE void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

/**
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static ZR_INLINE void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @param entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
#ifndef CONFIG_DEBUG_LIST
static ZR_INLINE void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = (struct list_head *)LIST_POISON1;
	entry->prev = (struct list_head *)LIST_POISON2;
}
#else
extern void list_del(struct list_head *entry);
#endif

/**
 * list_replace - replace old entry by new one
 * @param old : the element to be replaced
 * @param new : the new element to insert
 *
 * If @param old was empty, it will be overwritten.
 */
static ZR_INLINE void list_replace(struct list_head *old,
                                   struct list_head *new)
{
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}

static ZR_INLINE void list_replace_init(struct list_head *old,
                                        struct list_head *new)
{
	list_replace(old, new);
	INIT_LIST_HEAD(old);
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @param entry: the element to delete from the list.
 */
static ZR_INLINE void list_del_init(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry);
}

/**
 * list_move - delete from one list and add as another's head
 * @param list: the entry to move
 * @param head: the head that will precede our entry
 */
static ZR_INLINE void list_move(struct list_head *list, struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @param list: the entry to move
 * @param head: the head that will follow our entry
 */
static ZR_INLINE void list_move_tail(struct list_head *list,
                                     struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add_tail(list, head);
}

/**
 * list_is_last - tests whether @param list is the last entry in list @param head
 * @param list: the entry to test
 * @param head: the head of the list
 */
static ZR_INLINE int list_is_last(const struct list_head *list,
                                  const struct list_head *head)
{
	return list->next == head;
}

/**
 * list_empty - tests whether a list is empty
 * @param head: the list to test.
 */
static ZR_INLINE int list_empty(const struct list_head *head)
{
	return head->next == head;
}

/**
 * list_empty_careful - tests whether a list is empty and not being modified
 * @param head: the list to test
 *
 * Description:
 * tests whether a list is empty _and_ checks that no other CPU might be
 * in the process of modifying either member (next or prev)
 *
 * NOTE: using list_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is list_del_init(). Eg. it cannot be used
 * if another CPU could re-list_add() it.
 */
static ZR_INLINE int list_empty_careful(const struct list_head *head)
{
	struct list_head *next = head->next;
	return (next == head) && (next == head->prev);
}

/**
 * list_is_singular - tests whether a list has just one entry.
 * @param head: the list to test.
 */
static ZR_INLINE int list_is_singular(const struct list_head *head)
{
	return !list_empty(head) && (head->next == head->prev);
}

static ZR_INLINE void __list_cut_position(struct list_head *list,
                                          struct list_head *head,
                                          struct list_head *entry)
{
	struct list_head *new_first = entry->next;
	list->next = head->next;
	list->next->prev = list;
	list->prev = entry;
	entry->next = list;
	head->next = new_first;
	new_first->prev = head;
}

/**
 * list_cut_position - cut a list into two
 * @param list: a new list to add all removed entries
 * @param head: a list with entries
 * @param entry: an entry within head, could be the head itself
 *	and if so we won't cut the list
 *
 * This helper moves the initial part of @param head, up to and
 * including @param entry, from @param head to @param list. You should
 * pass on @param entry an element you know is on @param head. @param list
 * should be an empty list or a list you do not care about
 * losing its data.
 *
 */
static ZR_INLINE void list_cut_position(struct list_head *list,
                                        struct list_head *head,
                                        struct list_head *entry)
{
	if (list_empty(head))
		return;
	if (list_is_singular(head) &&
		(head->next != entry && head != entry))
		return;
	if (entry == head)
		INIT_LIST_HEAD(list);
	else
		__list_cut_position(list, head, entry);
}

static ZR_INLINE void __list_splice(const struct list_head *list,
                                    struct list_head *prev,
                                    struct list_head *next)
{
	struct list_head *first = list->next;
	struct list_head *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

/**
 * list_splice - join two lists, this is designed for stacks
 * @param list: the new list to add.
 * @param head: the place to add it in the first list.
 */
static ZR_INLINE void list_splice(const struct list_head *list,
                                  struct list_head *head)
{
	if (!list_empty(list))
		__list_splice(list, head, head->next);
}

/**
 * list_splice_tail - join two lists, each list being a queue
 * @param list: the new list to add.
 * @param head: the place to add it in the first list.
 */
static ZR_INLINE void list_splice_tail(struct list_head *list,
                                       struct list_head *head)
{
	if (!list_empty(list))
		__list_splice(list, head->prev, head);
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @param list: the new list to add.
 * @param head: the place to add it in the first list.
 *
 * The list at @param list is reinitialised
 */
static ZR_INLINE void list_splice_init(struct list_head *list,
                                       struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head, head->next);
		INIT_LIST_HEAD(list);
	}
}

/**
 * list_splice_tail_init - join two lists and reinitialise the emptied list
 * @param list: the new list to add.
 * @param head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @param list is reinitialised
 */
static ZR_INLINE void list_splice_tail_init(struct list_head *list,
                                            struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head->prev, head);
		INIT_LIST_HEAD(list);
	}
}

/**
 * list_entry - get the struct for this entry
 * @param ptr:          the &struct list_head pointer.
 * @param type:         the type of the struct this is embedded in.
 * @param member:       the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @param ptr:          the list head to take the element from.
 * @param type:         the type of the struct this is embedded in.
 * @param member:       the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/**
 * list_for_each        -       iterate over a list
 * @param pos:  the &struct list_head to use as a loop cursor.
 * @param head: the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; prefetch(pos->next), pos != (head); \
		pos = pos->next)

/**
 * __list_for_each      -       iterate over a list
 * @param pos:  the &struct list_head to use as a loop cursor.
 * @param head: the head for your list.
 *
 * This variant differs from list_for_each() in that it's the
 * simplest possible list iteration code, no prefetching is done.
 * Use this for code that knows the list to be very short (empty
 * or 1 entry) most of the time.
 */
#define __list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev   -       iterate over a list backwards
 * @param pos:  the &struct list_head to use as a loop cursor.
 * @param head: the head for your list.
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; prefetch(pos->prev), pos != (head); \
		pos = pos->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @param pos:  the &struct list_head to use as a loop cursor.
 * @param n:    another &struct list_head to use as temporary storage
 * @param head: the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @param pos:  the &struct list_head to use as a loop cursor.
 * @param n:    another &struct list_head to use as temporary storage
 * @param head: the head for your list.
 */
#define list_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev; \
		prefetch(pos->prev), pos != (head); \
		pos = n, n = pos->prev)

/**
 * list_for_each_entry  -       iterate over list of given type
 * @param pos:          the type * to use as a loop cursor.
 * @param head:         the head for your list.
 * @param member:       the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member)                          \
	for (pos = list_entry((head)->next, typeof(*pos), member);      \
		prefetch(pos->member.next), &pos->member != (head);     \
		pos = list_entry(pos->member.next, typeof(*pos), member))

/**
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @param pos:          the type * to use as a loop cursor.
 * @param head:         the head for your list.
 * @param member:       the name of the list_struct within the struct.
 */
#define list_for_each_entry_reverse(pos, head, member)                  \
	for (pos = list_entry((head)->prev, typeof(*pos), member);      \
		prefetch(pos->member.prev), &pos->member != (head);     \
		pos = list_entry(pos->member.prev, typeof(*pos), member))

/**
 * list_prepare_entry - prepare a pos entry for use in list_for_each_entry_continue()
 * @param pos:          the type * to use as a start point
 * @param head:         the head of the list
 * @param member:       the name of the list_struct within the struct.
 *
 * Prepares a pos entry for use as a start point in list_for_each_entry_continue().
 */
#define list_prepare_entry(pos, head, member) \
	((pos) ? : list_entry(head, typeof(*pos), member))

/**
 * list_for_each_entry_continue - continue iteration over list of given type
 * @param pos:          the type * to use as a loop cursor.
 * @param head:         the head for your list.
 * @param member:       the name of the list_struct within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define list_for_each_entry_continue(pos, head, member)                 \
	for (pos = list_entry(pos->member.next, typeof(*pos), member);  \
		prefetch(pos->member.next), &pos->member != (head);     \
		pos = list_entry(pos->member.next, typeof(*pos), member))

/**
 * list_for_each_entry_continue_reverse - iterate backwards from the given point
 * @param pos:          the type * to use as a loop cursor.
 * @param head:         the head for your list.
 * @param member:       the name of the list_struct within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define list_for_each_entry_continue_reverse(pos, head, member)         \
	for (pos = list_entry(pos->member.prev, typeof(*pos), member);  \
		prefetch(pos->member.prev), &pos->member != (head);     \
		pos = list_entry(pos->member.prev, typeof(*pos), member))

/**
 * list_for_each_entry_from - iterate over list of given type from the current point
 * @param pos:          the type * to use as a loop cursor.
 * @param head:         the head for your list.
 * @param member:       the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define list_for_each_entry_from(pos, head, member)                     \
	for (; prefetch(pos->member.next), &pos->member != (head);      \
		pos = list_entry(pos->member.next, typeof(*pos), member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @param pos:          the type * to use as a loop cursor.
 * @param n:            another type * to use as temporary storage
 * @param head:         the head for your list.
 * @param member:       the name of the list_struct within the struct.
 */
#define list_for_each_entry_safe(pos, n, head, member)                  \
	for (pos = list_entry((head)->next, typeof(*pos), member),      \
		    n = list_entry(pos->member.next, typeof(*pos), member);\
		&pos->member != (head);                                 \
		pos = n, n = list_entry(n->member.next, typeof(*n), member))

/**
 * list_for_each_entry_safe_continue
 * @param pos:          the type * to use as a loop cursor.
 * @param n:            another type * to use as temporary storage
 * @param head:         the head for your list.
 * @param member:       the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define list_for_each_entry_safe_continue(pos, n, head, member)         \
	for (pos = list_entry(pos->member.next, typeof(*pos), member),  \
		    n = list_entry(pos->member.next, typeof(*pos), member);\
		&pos->member != (head);                                 \
		pos = n, n = list_entry(n->member.next, typeof(*n), member))

/**
 * list_for_each_entry_safe_from
 * @param pos:          the type * to use as a loop cursor.
 * @param n:            another type * to use as temporary storage
 * @param head:         the head for your list.
 * @param member:       the name of the list_struct within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define list_for_each_entry_safe_from(pos, n, head, member)             \
	for (n = list_entry(pos->member.next, typeof(*pos), member);    \
		&pos->member != (head);                                 \
		pos = n, n = list_entry(n->member.next, typeof(*n), member))

/**
 * list_for_each_entry_safe_reverse
 * @param pos:          the type * to use as a loop cursor.
 * @param n:            another type * to use as temporary storage
 * @param head:         the head for your list.
 * @param member:       the name of the list_struct within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define list_for_each_entry_safe_reverse(pos, n, head, member)          \
	for (pos = list_entry((head)->prev, typeof(*pos), member),      \
		    n = list_entry(pos->member.prev, typeof(*pos), member);\
		&pos->member != (head);                                 \
		pos = n, n = list_entry(n->member.prev, typeof(*n), member))

/*
 * Double linked lists with a single pointer list head.
 * Mostly useful for hash tables where the two pointer list head is
 * too wasteful.
 * You lose the ability to access the tail in O(1).
 */

struct hlist_head {
	struct hlist_node *first;
};

struct hlist_node {
	struct hlist_node *next, **pprev;
};

#define HLIST_HEAD_INIT { .first = NULL }
#define HLIST_HEAD(name) struct hlist_head name = {  .first = NULL }
#define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)
static ZR_INLINE void INIT_HLIST_NODE(struct hlist_node *h)
{
	h->next = NULL;
	h->pprev = NULL;
}

static ZR_INLINE int hlist_unhashed(const struct hlist_node *h)
{
	return !h->pprev;
}

static ZR_INLINE int hlist_empty(const struct hlist_head *h)
{
	return !h->first;
}

static ZR_INLINE void __hlist_del(struct hlist_node *n)
{
	struct hlist_node *next = n->next;
	struct hlist_node **pprev = n->pprev;
	*pprev = next;
	if (next)
		next->pprev = pprev;
}

static ZR_INLINE void hlist_del(struct hlist_node *n)
{
	__hlist_del(n);
	n->next = (struct hlist_node *)LIST_POISON1;
	n->pprev = (struct hlist_node **)LIST_POISON2;
}

static ZR_INLINE void hlist_del_init(struct hlist_node *n)
{
	if (!hlist_unhashed(n)) {
		__hlist_del(n);
		INIT_HLIST_NODE(n);
	}
}

static ZR_INLINE void hlist_add_head(struct hlist_node *n, struct hlist_head *h)
{
	struct hlist_node *first = h->first;
	n->next = first;
	if (first)
		first->pprev = &n->next;
	h->first = n;
	n->pprev = &h->first;
}

/* next must be != NULL */
static ZR_INLINE void hlist_add_before(struct hlist_node *n,
                                       struct hlist_node *next)
{
	n->pprev = next->pprev;
	n->next = next;
	next->pprev = &n->next;
	*(n->pprev) = n;
}

static ZR_INLINE void hlist_add_after(struct hlist_node *n,
                                      struct hlist_node *next)
{
	next->next = n->next;
	n->next = next;
	next->pprev = &n->next;

	if(next->next)
		next->next->pprev  = &next->next;
}

/*
 * Move a list from one list head to another. Fixup the pprev
 * reference of the first entry if it exists.
 */
static ZR_INLINE void hlist_move_list(struct hlist_head *old,
                                      struct hlist_head *new)
{
	new->first = old->first;
	if (new->first)
		new->first->pprev = &new->first;
	old->first = NULL;
}

#define hlist_entry(ptr, type, member) container_of(ptr,type,member)

#define hlist_for_each(pos, head) \
	for (pos = (head)->first; pos && ({ prefetch(pos->next); 1; }); \
		pos = pos->next)

#define hlist_for_each_safe(pos, n, head) \
	for (pos = (head)->first; pos && ({ n = pos->next; 1; }); \
		pos = n)

/**
 * hlist_for_each_entry - iterate over list of given type
 * @param tpos:         the type * to use as a loop cursor.
 * @param pos:          the &struct hlist_node to use as a loop cursor.
 * @param head:         the head for your list.
 * @param member:       the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry(tpos, pos, head, member)                    \
	for (pos = (head)->first;                                        \
		pos && ({ prefetch(pos->next); 1;}) &&                   \
		    ({ tpos = hlist_entry(pos, typeof(*tpos), member); 1;}); \
		pos = pos->next)

/**
 * hlist_for_each_entry_continue - iterate over a hlist continuing after current point
 * @param tpos:         the type * to use as a loop cursor.
 * @param pos:          the &struct hlist_node to use as a loop cursor.
 * @param member:       the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_continue(tpos, pos, member)                 \
	for (pos = (pos)->next;                                          \
		pos && ({ prefetch(pos->next); 1;}) &&                   \
		    ({ tpos = hlist_entry(pos, typeof(*tpos), member); 1;}); \
		pos = pos->next)

/**
 * hlist_for_each_entry_from - iterate over a hlist continuing from current point
 * @param tpos:         the type * to use as a loop cursor.
 * @param pos:          the &struct hlist_node to use as a loop cursor.
 * @param member:       the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_from(tpos, pos, member)                     \
	for (; pos && ({ prefetch(pos->next); 1;}) &&                    \
		    ({ tpos = hlist_entry(pos, typeof(*tpos), member); 1;}); \
		pos = pos->next)

/**
 * hlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @param tpos:         the type * to use as a loop cursor.
 * @param pos:          the &struct hlist_node to use as a loop cursor.
 * @param n:            another &struct hlist_node to use as temporary storage
 * @param head:         the head for your list.
 * @param member:       the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_safe(tpos, pos, n, head, member)            \
	for (pos = (head)->first;                                        \
		pos && ({ n = pos->next; 1; }) &&                        \
		    ({ tpos = hlist_entry(pos, typeof(*tpos), member); 1;}); \
		pos = n)

/////////////////////////////////////// Circular ///////////////////////////////////////

/*
 * Circular queue definitions.
 */
#define LIST_CIRCLEQ_HEAD(name, type)                                   \
struct name {                                                           \
	struct type *cqh_first;         /* first element */             \
	struct type *cqh_last;          /* last element */              \
}

#define LIST_CIRCLEQ_HEAD_INITIALIZER(head)                             \
	{ LIST_CIRCLEQ_END(&head), LIST_CIRCLEQ_END(&head) }

#define LIST_CIRCLEQ_ENTRY(type)                                        \
struct {                                                                \
	struct type *cqe_next;          /* next element */              \
	struct type *cqe_prev;          /* previous element */          \
}

/*
 * Circular queue access methods
 */
#define LIST_CIRCLEQ_FIRST(head)        ((head)->cqh_first)
#define LIST_CIRCLEQ_LAST(head)         ((head)->cqh_last)
#define LIST_CIRCLEQ_END(head)          ((void *)(head))
#define LIST_CIRCLEQ_NEXT(elm, field)   ((elm)->field.cqe_next)
#define LIST_CIRCLEQ_PREV(elm, field)   ((elm)->field.cqe_prev)
#define LIST_CIRCLEQ_EMPTY(head)                                \
	(LIST_CIRCLEQ_FIRST(head) == LIST_CIRCLEQ_END(head))

#define LIST_CIRCLEQ_EMPTY_ENTRY(elm, field) (((elm)->field.cqe_next == NULL) && ((elm)->field.cqe_prev == NULL))

#define LIST_CIRCLEQ_FOREACH(var, head, field)                          \
	for((var) = LIST_CIRCLEQ_FIRST(head);                           \
		(var) != LIST_CIRCLEQ_END(head);                        \
		(var) = LIST_CIRCLEQ_NEXT(var, field))

#define LIST_CIRCLEQ_FOREACH_SAFE(var, var2, head, field)               \
	for((var) = LIST_CIRCLEQ_FIRST(head), var2 = LIST_CIRCLEQ_NEXT(var, field);\
		(var) != LIST_CIRCLEQ_END(head);                        \
		(var) = var2, var2 = LIST_CIRCLEQ_NEXT(var, field))

#define LIST_CIRCLEQ_FOREACH_REVERSE(var, head, field)                  \
	for((var) = LIST_CIRCLEQ_LAST(head);                            \
		(var) != LIST_CIRCLEQ_END(head);                        \
		(var) = LIST_CIRCLEQ_PREV(var, field))

/*
 * Circular queue functions.
 */
#define LIST_CIRCLEQ_INIT(head) do {                                    \
	(head)->cqh_first = LIST_CIRCLEQ_END(head);                     \
	(head)->cqh_last = LIST_CIRCLEQ_END(head);                      \
} while (0)

#define LIST_CIRCLEQ_INIT_ENTRY(elm, field) do { \
	(elm)->field.cqe_next = NULL; \
	(elm)->field.cqe_prev = NULL; \
} while (0)

#define LIST_CIRCLEQ_INSERT_AFTER(head, listelm, elm, field) do {       \
	(elm)->field.cqe_next = (listelm)->field.cqe_next;              \
	(elm)->field.cqe_prev = (listelm);                              \
	if ((listelm)->field.cqe_next == LIST_CIRCLEQ_END(head))        \
		(head)->cqh_last = (elm);                               \
	else                                                            \
		(listelm)->field.cqe_next->field.cqe_prev = (elm);      \
	(listelm)->field.cqe_next = (elm);                              \
} while (0)

#define LIST_CIRCLEQ_INSERT_BEFORE(head, listelm, elm, field) do {      \
	(elm)->field.cqe_next = (listelm);                              \
	(elm)->field.cqe_prev = (listelm)->field.cqe_prev;              \
	if ((listelm)->field.cqe_prev == LIST_CIRCLEQ_END(head))        \
		(head)->cqh_first = (elm);                              \
	else                                                            \
		(listelm)->field.cqe_prev->field.cqe_next = (elm);      \
	(listelm)->field.cqe_prev = (elm);                              \
} while (0)

#define LIST_CIRCLEQ_INSERT_HEAD(head, elm, field) do {                 \
	(elm)->field.cqe_next = (head)->cqh_first;                      \
	(elm)->field.cqe_prev = LIST_CIRCLEQ_END(head);                 \
	if ((head)->cqh_last == LIST_CIRCLEQ_END(head))                 \
		(head)->cqh_last = (elm);                               \
	else                                                            \
		(head)->cqh_first->field.cqe_prev = (elm);              \
	(head)->cqh_first = (elm);                                      \
} while (0)

#define LIST_CIRCLEQ_INSERT_TAIL(head, elm, field) do {                 \
	(elm)->field.cqe_next = LIST_CIRCLEQ_END(head);                 \
	(elm)->field.cqe_prev = (head)->cqh_last;                       \
	if ((head)->cqh_first == LIST_CIRCLEQ_END(head))                \
		(head)->cqh_first = (elm);                              \
	else                                                            \
		(head)->cqh_last->field.cqe_next = (elm);               \
	(head)->cqh_last = (elm);                                       \
} while (0)

#define LIST_CIRCLEQ_REMOVE(head, elm, field) do {                      \
	if ((elm)->field.cqe_next == LIST_CIRCLEQ_END(head))            \
		(head)->cqh_last = (elm)->field.cqe_prev;               \
	else                                                            \
		(elm)->field.cqe_next->field.cqe_prev =                 \
		    (elm)->field.cqe_prev;                              \
	if ((elm)->field.cqe_prev == LIST_CIRCLEQ_END(head))            \
		(head)->cqh_first = (elm)->field.cqe_next;              \
	else                                                            \
		(elm)->field.cqe_prev->field.cqe_next =                 \
		    (elm)->field.cqe_next;                              \
} while (0)

#define LIST_CIRCLEQ_REMOVE_INIT(head, elm, field) do { \
	LIST_CIRCLEQ_REMOVE(head, elm, field); \
	LIST_CIRCLEQ_INIT_ENTRY(elm, field); \
} while (0)

#define LIST_CIRCLEQ_REPLACE(head, elm, elm2, field) do {               \
	if (((elm2)->field.cqe_next = (elm)->field.cqe_next) ==         \
	    LIST_CIRCLEQ_END(head))                                     \
		(head).cqh_last = (elm2);                               \
	else                                                            \
		(elm2)->field.cqe_next->field.cqe_prev = (elm2);        \
	if (((elm2)->field.cqe_prev = (elm)->field.cqe_prev) ==         \
	    LIST_CIRCLEQ_END(head))                                     \
		(head).cqh_first = (elm2);                              \
	else                                                            \
		(elm2)->field.cqe_prev->field.cqe_next = (elm2);        \
} while (0)



#ifdef __cplusplus
}
#endif

#endif

