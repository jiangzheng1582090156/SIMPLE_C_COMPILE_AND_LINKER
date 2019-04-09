#ifndef __KSTACK_H__
#define __KSTACK_H__


struct stack_node {
	struct stack_node *next;
};

struct stack_base {
	struct stack_node base;
	struct stack_node *top;
};

/*
 * offset - 计算结构体中的某成员的偏移量
 * @type: 结构体的类型
 * @member: 需要计算偏移量的成员
 */
#define offset(type,member) \
	(size_t)(&(((type *)0)->member))
/*
 * container_of - 得到包含该成员结构体的地址
 * @ptr: 该成员的地址
 * @type: 这个结构体的类型
 * @member: ptr地址所指向成员的名字
 */

#ifndef __LIST_H__
#define container_of(ptr,type,member) \
	(type *)((char *)ptr - offset(type,member))
#endif // __LIST_H__

#define STACK_BASE(name) \
	struct stack_base name = { { NULL }, &name.base }

/*
 * stack_is_empty - 判断栈是否为空
 * @base: 栈指针
 */
static inline int
stack_is_empty(struct stack_base *base)
{
	return (base->top == &(base->base));
}

/*
 * stack_push - 入栈操作
 * @n: 入栈结点指针
 * @base: 栈指针
 */
static inline void
stack_push(struct stack_node *n, struct stack_base *base)
{
	n->next = base->top;
	base->top = n;
}

/*
 * stack_pop - 出栈操作
 * @base: 栈指针
 */
static inline struct stack_node *
stack_pop(struct stack_base *base)
{
	struct stack_node *node = base->top;
	base->top = base->top->next;
	return node;
}

static inline struct stack_node *
stack_top(struct stack_base *base)
{
	return base->top;
}

static inline void
stack_clear(struct stack_base *base)
{
	while (!stack_is_empty(base)) {
		stack_pop(base);
	}
}

/*
 * stack_pop_entry - 弹出对应的结构体指针
 * @base: 栈的指针
 * @type: 结构体得类型
 * @member: 在结构体中stack_node成员对应的名字
 */
#define stack_pop_entry(base,type,member) \
	container_of(stack_pop(base),type,member)

#define stack_top_entry(base,type,member) \
 	container_of(stack_top(base),type,member)


#endif   //__KSTACK_H__
