#ifndef __ENUME_CODE_H__
#define __ENUME_CODE_H__


/* 单词编码 */
enum e_token_code {
        ETC_PLUS,       /* + */
        ETC_MINUS,      /* - */
        ETC_STAR,       /* * */
        ETC_DIVIDE,     /* / */
        ETC_MOD,        /* % */
        ETC_EQ,         /* == */
        ETC_NEQ,        /* != */
        ETC_LT,         /* < */
        ETC_LEQ,        /* <= */
        ETC_GT,         /* > */
        ETC_GEQ,        /* >= */
        ETC_ASSIGN,     /* = */
        ETC_POINTTO,    /* -> */
        ETC_DOT,        /* . */
        ETC_AND,        /* & */
        ETC_OPENPA,     /* ( */
        ETC_CLOSEPA,    /* ) */
        ETC_OPENBR,     /* [ */
        ETC_CLOSEBR,    /* ] */
        ETC_OPENBRA,    /* { */
        ETC_CLOSEBRA,   /* } */
        ETC_SEMCOLON,   /* ; */
        ETC_COMMA,      /* , */
        ETC_ELLPSIS,    /* ... */
        ETC_EOF,        /* 文件结束符 */
        ETC_CINT,       /* 整形常量 */
        ETC_CCHAR,      /* 字符常量 */
        ETC_CSTR,       /* 字符串常量 */
        ETC_KW_CHAR,
        ETC_KW_SHORT,
        ETC_KW_INT,
        ETC_KW_VOID,
        ETC_KW_STRUCT,
        ETC_KW_IF,
        ETC_KW_ELSE,
        ETC_KW_FOR,
        ETC_KW_CONTINUE,
        ETC_KW_BREAK,
        ETC_KW_RETURN,
        ETC_KW_SIZEOF,
        ETC_KW_ALIGN,
        ETC_KW_CDECL,
        ETC_KW_STDCALL,

        ETC_KW_IDENT
};


enum e_storage_type {
        SC_GLOBAL       = 0x00f0,       //包括整型常量、字符常量、字符串常量、全局变量、函数定义
        SC_LOCAL        = 0x00f1,       //栈中的变量
        SC_LLOCAL       = 0x00f2,       //寄存器溢出存放栈中
        SC_CMP          = 0x00f3,       //使用标志寄存器
        SC_VALMASK      = 0x00ff,       //存储类型掩码
        SC_LVAL         = 0x0100,       //左值
        SC_SYM          = 0x0200,       //符号

        SC_ANOM         = 0x10000000,   //匿名符号
        SC_STRUCT       = 0x20000000,   //结构体符号
        SC_MEMBER       = 0x40000000,   //结构体成员变量

        SC_PARAMS       = 0x80000000    //函数参数
};

/**
 * 数据类型编码
 */
enum e_code_type {
        TYPE_INT        = 0,
        TYPE_CHAR       = 1,
        TYPE_SHORT      = 2,
        TYPE_VOID       = 3,
        TYPE_PTR        = 4,
        TYPE_FUNC       = 5,
        TYPE_STRUCT     = 6,

        TYPE_MASK       = 0x000f,  //基本数据类型掩码
        TYPE_ARRAY      = 0x0010   //数组
};

// 强制对齐宏定义
#define ALIGN_SET       0x100


#endif // __ENUME_CODE_H__
