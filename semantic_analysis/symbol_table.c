#include "symbol_table.h"
#include "../whole_value.h"
#include "../word_analysis/word_table.h"
#include "../error/error.h"
#include <stdio.h>
#include <malloc.h>
#include <memory.h>

/**
 * 功能: 将符号放入符号栈中
 * @param  base 栈结构体指针
 * @param  v    符号编码
 * @param  type 符号数据类型
 * @param  c    符号关联的值
 * @return      入栈结构体的指针
 */
struct
symbol *__sym_push(struct stack_base *base, int v,
                        struct type *type, int c)
{
        struct symbol *push_sym = NULL;
        push_sym = malloc(sizeof(*push_sym));
        memset(push_sym, 0, sizeof(*push_sym));

        push_sym->v             = v;
        push_sym->type.t        = type->t;
        push_sym->type.ref      = type->ref;
        push_sym->c             = c;
        push_sym->next          = NULL;

        stack_push(&push_sym->node, base);
        return push_sym;
}


/**
 * 功能: 将符号放入栈中，判断是全局栈还是局部栈
 * @param  v    符号编号
 * @param  type 符号数据类型
 * @param  r    符号存储类型
 * @param  c    符号关联的值
 * @return      入栈接结构体的指针
 */
struct symbol *
sym_push(int v, struct type *type, int r, int c)
{
        struct symbol *ps, **pps;
        struct tk_world *ts;
        struct stack_base *base;

        if (stack_is_empty(&(local_sym_stack)) == 0) {
                base = &(local_sym_stack);
        } else {
                base = &(global_sym_stack);
        }

        ps = __sym_push(base, v, type, c);
        ps->r = r;

        //不记录结构体成员
        if ((v & SC_STRUCT) || v < SC_ANOM) {
                ts = (struct tk_world *)tktable.array[(v & ~SC_STRUCT)];
                if (v & SC_STRUCT)
                        pps = &(ts->sym_struct);
                else
                        pps = &(ts->sym_identifier);

                ps->prev_tok = *pps;
                *pps = ps;
        }

        return ps;
}

/**
 * 功能: 将函数放入全符号栈
 * @param  v    符号编号
 * @param  type 符号数据类型
 * @return      symbol指针
 */
struct symbol *
func_sym_push(int v, struct type *type)
{
        struct symbol *s, **ps;

        s = __sym_push(&global_sym_stack, v, type, 0);

        ps = &((struct tk_world *)tktable.array[v])->sym_identifier;

        //同名符号函数符号放在最后
        while (*ps != NULL)
                ps = &(*ps)->prev_tok;

        s->prev_tok = NULL;
        *ps = s;
        return s;
}




struct symbol *
var_sym_push(struct type* type, int r, int v, int addr)
{
        struct symbol *sym = NULL;
        if ((r & SC_VALMASK) == SC_LOCAL) {
                // 局部变量
                sym = sym_push(v, type, r, addr);
        } else if (v && (r & SC_VALMASK) == SC_GLOBAL) {
                // 全局变量
                sym = sym_search(v);
                if (sym) {
                        error("%s重定义", ((struct tk_world *)tktable.array[v])->spelling);
                } else {
                        sym = sym_push(v, type, r | SC_SYM, 0);
                }
        }

        return sym;
}

/**
 * 功能: 将节名称放入全局符号表
 * @param  src 节名称
 * @param  c   符号关联值
 * @return     入栈结构体指针
 */
struct symbol *
sec_sym_put(char *sec, int c)
{
        struct tk_world *tp;
        struct symbol *s;
        struct type type;
        type.t = TYPE_INT;
        tp = tkworld_insert(sec);
        token = tp->etc_code;
        s = sym_push(token, &type, SC_GLOBAL, c);
        return s;
}



/**
 * 功能: 删除栈中的符号，知道栈顶符号为'b'
 * @param base 栈结构体指针
 * @param b    符号指针
 */
void
sym_pop(struct stack_base *base, struct symbol *b)
{
        struct symbol *s, **ps;
        struct tk_world *ts;
        int v;
        s =  stack_top_entry(base, struct symbol, node);
        while (s != b) {
                v = s->v;
                // 更新单词表中的sym_struct 和 sym_identifier
                if ((v & SC_STRUCT) || v < SC_ANOM) {
                        ts = (struct tk_world *)tktable.array[(v & ~SC_STRUCT)];
                        if (v & SC_STRUCT)
                                ps = &(ts->sym_struct);
                        else
                                ps = &(ts->sym_identifier);
                        *ps = s->prev_tok;
                }

                // 弹出s
                stack_pop(base);
                s = stack_top_entry(base, struct symbol, node);
        }
}

/**
 * 功能: 查找结构定义
 * @param  v 符号编号
 * @return   查找结果的结构体指针
 */
struct symbol *
struct_search(int v)
{
        if (v >= tktable.size) {
                return NULL;
        } else {
                return ((struct tk_world *)tktable.array[v])->sym_struct;
        }
}

/**
 * 功能: 查找结构定义
 * @param  v 符号编号
 * @return   查找结果的结构体指针
 */
struct symbol *
sym_search(int v)
{
        if (v >= tktable.size) {
                return NULL;
        } else {
                return ((struct tk_world *)tktable.array[v])->sym_identifier;
        }
}
