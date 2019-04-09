#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include "../stack/kstack.h"

struct symbol;

struct type {
        int t;
        struct symbol *ref;
};

struct symbol {
        struct stack_node node; // 栈节点
        int v;                  // 符号的单词编码
        int r;                  // 符号关联的寄存器
        int c;                  // 符号关联的值
        struct type type;       // 符号数据类型
        struct symbol *next;    // 关联的其他符号
        struct symbol *prev_tok;// 指向前一定义的同名符号
};


/**
 * 功能: 将符号放入符号栈中
 * @param  base 栈结构体指针
 * @param  v    符号编码
 * @param  type 符号数据类型
 * @param  c    符号关联的值
 * @return      入栈结构体的指针
 */
struct symbol *__sym_push(struct stack_base *base, int v, struct type *type, int c);

/**
 * 功能: 将符号放入栈中，判断是全局栈还是局部栈
 * @param  v    符号编号
 * @param  type 符号数据类型
 * @param  r    符号存储类型
 * @param  c    符号关联的值
 * @return      入栈接结构体的指针
 */
struct symbol *sym_push(int v, struct type *type, int r, int c);

/**
 * 功能: 将函数放入全符号栈
 * @param  v    符号编号
 * @param  type 符号数据类型
 * @return      symbol指针
 */
struct symbol *func_sym_push(int v, struct type *type);


struct symbol *var_sym_push(struct type* type, int r, int v, int addr);

/**
 * 功能: 将节名称放入全局符号表
 * @param  src 节名称
 * @param  c   符号关联值
 * @return     入栈结构体指针
 */
struct symbol *sec_sym_put(char *src, int c);

/**
 * 功能: 删除栈中的符号，知道栈顶符号为'b'
 * @param base 栈结构体指针
 * @param b    符号指针
 */
void sym_pop(struct stack_base *base, struct symbol *b);

/**
 * 功能: 查找结构定义
 * @param  v 符号编号
 * @return   查找结果的结构体指针
 */
struct symbol *struct_search(int v);

/**
 * 功能: 查找结构定义
 * @param  v 符号编号
 * @return   查找结果的结构体指针
 */
struct symbol *sym_search(int v);


#endif // __SYMBOL_TABLE_H__
