#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

struct symbol;

struct type {
        int t;
        struct symbol *ref;
};

struct symbol {
        int v;                  // 符号的单词编码
        int r;                  // 符号关联的寄存器
        int c;                  // 符号关联的值
        struct type type;       // 符号数据类型
        struct symbol *next;    // 关联的其他符号
        struct symbol *prev_tok;// 指向前一定义的同名符号
};

#endif // __SYMBOL_TABLE_H__
