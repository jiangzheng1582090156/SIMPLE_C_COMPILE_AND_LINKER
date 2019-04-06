#ifndef __LIST_H__
#define __LIST_H__

#include <stdio.h>

struct list_head {
	struct list_head *next, *prev;
};

/**
 *  LIST_HEAD_INIT - create and initialize a node
 * @name: struct name
 */
#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)

/**
 * INIT_LIST_HEAD - initialize a node then you create
 * @ptr: the pointer of list_head member
 */
#define INIT_LIST_HEAD(ptr) \
    do{ \
        (ptr)->next = ptr; (ptr)->prev = ptr; \
    }while(0)

/**
 * offset - caculate the offset of a member
 * @type: type of the structure
 * @member: member of the structure
 */
#define offsetof(type,member) \
    ((size_t) &((type *)0)->member)

/**
 * container_of - caculate the address of structure in which member is located
 * @ptr: address of the member
 * @type: structure's type
 * @member:member of structure
 */
#define container_of(ptr,type,member) (  \
    (type *) ((char *)ptr - offsetof(type,member)))

/* add list node */
static inline void
__list_add(struct list_head *new_node,
	   struct list_head *prev, struct list_head *next)
{
	new_node->next = next;
	new_node->prev = prev;
	next->prev = new_node;
	prev->next = new_node;
}

/* head insertion */
static inline void
list_add(struct list_head *new_node, struct list_head *head)
{
	__list_add(new_node, head, head->next);
}

/* tail insertion */
static inline void
list_add_tail(struct list_head *new_node, struct list_head *head)
{
	__list_add(new_node, head->prev, head);
}


static inline void
__list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void __list_del_entry(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
}

static inline void list_del(struct list_head *entry)
{
	__list_del_entry(entry);
	entry->next = NULL;
	entry->prev = NULL;
}

static inline void
list_replace(struct list_head *old_node, struct list_head *new_node)
{
	new_node->next = old_node->next;
	new_node->next->prev = new_node;
	new_node->prev = old_node->prev;
	new_node->prev->next = new_node;
}


static inline void
list_replace_init(struct list_head *old_node, struct list_head *new_node)
{
	list_replace(old_node, new_node);
	INIT_LIST_HEAD(old_node);
}

/**
 * delete the entry and reintialize it;
 * @entry: the element to delete from the list
 */
static inline void list_del_init(struct list_head *entry)
{
	__list_del_entry(entry);
	INIT_LIST_HEAD(entry);
}

/**
 * list move: delete from one list and add as anothor list head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void
list_move(struct list_head *list, struct list_head *head)
{
	__list_del_entry(list);
	list_add(list, head);
}

/**
 * list move: delete from one list and add as anothor list head
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void
list_move_tail(struct list_head *list, struct list_head *head)
{
	__list_del_entry(list);
	list_add_tail(list, head);
}

/**
 * list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int
list_is_last(const struct list_head *list, const struct list_head *head)
{
	return list->next == head;
}

/**
 * list_empty: tests whether a list is empty
 * @head: the list to test
 */
static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

/**
 * list_rotate_left: rotate the list to the left
 * @head: the list head
 */
static inline void list_rotate_left(struct list_head *head)
{
	struct list_head *first;
	if (!list_empty(head)) {
		first = head->next;
		list_move_tail(first, head);
	}
}

static inline void
__list_cut_position(struct list_head *list,
		    struct list_head *head, struct list_head *entry)
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
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *      and if so we won't cut the list
 */
static inline void
list_cut_position(struct list_head *list,
		  struct list_head *head, struct list_head *entry)
{
	if (list_empty(head))
		return;
	if (head->next == head)
		return;
	if (head == entry) {
		INIT_LIST_HEAD(list);
	} else {
		__list_cut_position(list, head, entry);
	}
}

/**
 * __list_splice - insert the linked list between two nodes
 * @list: linked list needed to be insert
 * @prev: insert the previous node of the location
 * @next: insert the next node of the location
 */
static inline void
__list_splice(const struct list_head *list,
	      struct list_head *prev, struct list_head *next)
{
	struct list_head *first = list->next;
	struct list_head *last = list->prev;

	prev->next = first;
	first->prev = prev;

	next->prev = last;
	last->next = next;
}

/**
 * list_splice - Merging two linked lists into one linked list, the new list insert the head
 * @list: the list needed to be insert
 * @head: the list should be inserted
 */
static inline void
list_splice(const struct list_head *list, struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head, head->next);
	}
}

/**
 * list_splice - Merging two linked lists into one linked list, the new list insert the tail
 * @list: the list needed to be insert
 * @head: the list should be inserted
 */
static inline void
list_splice_tail(const struct list_head *list, struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head->prev, head);
	}
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void
list_splice_init(struct list_head *list, struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head, head->next);
		INIT_LIST_HEAD(list);
	}
}

/**
 * list_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void
list_splice_tail_init(struct list_head *list, struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head->prev, head);
		INIT_LIST_HEAD(list);
	}
}

/**
 * list_entry - get the struct of the entry
 * @ptr: the list head to take the element from
 * @type: structure's type
 * @member: the name of the list_head within the struct
 */
#define list_entry(ptr,type,member) \
    container_of(ptr,type,member)

/**
 * list_entry - get the first element from a list
 * @ptr: the list head to take the element from
 * @type: struct's type
 * @member:name of the list_head within struct
 */
#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

/**
 * list_entry - get the last element from a list
 * @ptr: the list head to take the element from
 * @type: struct's type
 * @member:name of the list_head within struct
 */
#define list_last_entry(ptr, type, member) \
    list_entry((ptr)->prev, type, member)

/**
 * list_next_entry - get the next element in list
 * @pos: the current type * of the struct
 * @member: the name of the list_head within the strcut
 */
#define list_next_entry(pos,member) \
    list_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * list_next_entry - get the prev element in list
 * @pos: the current type * of the struct
 * @member: the name of the list_head within the strcut
 */
#define list_prev_entry(pos,member) \
    list_entry((pos)->member.prev, typeof(*(pos)), member)


/**
 * list_for_each - iterator over a list
 * @pos: the pointer of the iterator
 * @head: the list head of a list
 */
#define list_for_each(pos,head) \
    for(pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each - iterator over a list backwords
 * @pos: the pointer of the iterator
 * @head: the list head of a list
 */
#define list_for_each_reverse(pos,head) \
    for(pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * list_for_each_entry - iterator over a list given a pointer of one entry
 * @pos: the pointer of the iterator
 * @head: the head of a list
 * @member: the name of list_head within struct
 */
#define list_for_each_entry(pos,head,member) \
    for(pos = list_first_entry(head, typeof(*pos), member); \
        &pos->member != (head);                             \
        pos = list_next_entry(pos, member))

/**
 * list_for_each_entry - iterator over a list given a pointer of one entry, backwords
 * @pos: the pointer of the iterator
 * @head: the head of a list
 * @member: the name of list_head within struct
 */
#define list_for_each_entry_reverse(pos,head,member) \
    for(pos = list_last_entry(head, typeof(*pos), member);  \
        &pos->member != (head);                             \
        pos = list_prev_entry(pos, member))

/*************************************************************************************
 *                           hashlist                                                *
 *************************************************************************************/

struct hlist_node {
	struct hlist_node *next;
	struct hlist_node **pprev;  /* Easier to modify the next pointer of the previous node */
};

struct hlist_head {
	struct hlist_node *first;
};

#define HLIST_HEAD_INIT {.first = NULL}
#define HLIST_HEAD(name) struct hlist_head name = HLIST_HEAD_INIT;
#define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)

static inline void
INIT_HLIST_NODE(struct hlist_node *h)
{
	h->next = NULL;
	h->pprev = NULL;
}

/*
 * hlist_unhashed - 判断节点是否已经插入哈希表
 * @h: 需要判断的节点
 */
static inline int
hlist_unhashed(const struct hlist_node *h)
{
	return !h->pprev;
}
/*
 * hlist_is_empty - 判断链表是否为空
 * @h: 哈希表头节点
 */
static inline int
hlist_is_empty(const struct hlist_head *h)
{
	return !h->first;
}

/*
 * hlist_add_head - 在头节点后添加
 * @h: 头节点
 * @n: 需要添加的节点
 */
static inline void
hlist_add_head(struct hlist_head *h, struct hlist_node *n)
{
	struct hlist_node *first_node = h->first;
	n->next = first_node;
	if(first_node)
		first_node->pprev = &n->next;

	h->first = n;
	n->pprev = &h->first;
}
/*
 * hlist_add_befor - 在某个节点之前添加
 * @n: 需要添加的节点
 * @next: 需要添加位置接地那的后一个节点
 */
static inline void
hlist_add_before(struct hlist_node *n, struct hlist_node *next)
{
	n->pprev = next->pprev;
	n->next = next;
	next->pprev = &n->next;
	*(n->pprev) = n;
}

/*
 * hlist_add_after - 在某个节点钱添加
 * @n: 插入的节点
 * @before: 哈希表中的原节点
 */
static inline void
hlist_add_after(struct hlist_node *n, struct hlist_node *before)
{
	n->next = before->next;
	before->next = n;
	n->pprev = &before->next;
	if(n->next)
		n->next->pprev = &n->next;
}

static inline void
__hlist_del(struct hlist_node *n)
{
	struct hlist_node *next = n->next;
	struct hlist_node **pprev = n->pprev;
	*pprev = next;
	if(next)
		next->pprev = pprev;
}
/*
 * hlist_del - 删除节点
 * @n: 删除的节点
 */
static inline void
hlist_del(struct hlist_node *n)
{
	__hlist_del(n);
}
static inline void
hlist_del_init(struct hlist_node *n)
{
	__hlist_del(n);
	INIT_HLIST_NODE(n);
}

/*
 * hlist_move_list - 移动哈希表, 从old头节点到new头节点
 * @old_head: 原先的哈希头节点
 * @new_head: 新的哈希表头节点
 */
static inline void
hlist_move_list(struct hlist_head *old_head, struct hlist_head *new_head)
{
	new_head->first = old_head->first;
	if(new_head->first)
		new_head->first->pprev = &new_head->first;
	old_head->first = NULL;
}

#define hlist_entry(ptr,type,member) \
	container_of(ptr,type,member);

#define hlist_for_each(pos,head) \
	for(pos = (head)->first; pos; pos = pos->next)

#define hlist_for_each_safe(pos,n,head) \
	for(pos = (head)->first; pos && ({n = pos->next;1;}); pos = n)
#define hlist_for_each_entry(tpos,pos,head,member) \
	for(pos = (head)->first; \
			pos && ({tpos = hlist_entry(pos,typeof(*tpos),member);1;});\
			pos = pos->next)

#define hlist_for_each_entry_safe(tpos,pos,n,head,member) \
	for(pos = (head)->first; \
			pos && ({n = pos->next;1;}) && \
			({tpos = hlist_entry(pos,typeof(*tpos),member);1;}); \
			pos = n)
#endif				//__LIST_H__
