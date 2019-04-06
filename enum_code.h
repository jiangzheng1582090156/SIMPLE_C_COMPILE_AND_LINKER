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


#define SC_GLOBAL       1
#define SC_LOCAL        2
#define SC_MEMBER       3

#endif // __ENUME_CODE_H__
