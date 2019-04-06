#include "./word_table.h"


#include <string.h>
#include <memory.h>
#include <malloc.h>
/**
* [elf_hash 计算hash值]
* @param  key [字符串]
* @return     [返回计算的keuno]
*/
int
elf_hash(char *key)
{
        int h = 0, g;
        while (*key) {
                h = (h << 4) + *key++;
                g = h & 0xf0000000;
                if (g)
                        h ^= g >> 24;
                h &= ~g;
        }

        return h % MAXKEY;
}


/**
 * [tk_word_direct_insert 运算符 关键字 常量 直接放入单词表]
 * @param  tp [description]
 * @return    [description]
 */
void tk_word_direct_insert(struct tk_world *tp)
{
        int keyno = elf_hash(tp->spelling);
        // printf("tp->spelling = %s, keyno = %d ", tp->spelling, keyno);
        array_add_point(&tktable, (void *)tp);
        // 插入对应的下标的链表
        list_add_tail(&(tp->node), &(tk_hashtable[keyno]));

        // if (strcmp("struct", tp->spelling) == 0)
        //         printf("struct keyno = %d\n", keyno);
        // printf("str =t %s\n", list_entry((tk_hashtable[keyno].next), struct tk_world, node)->spelling);
}


/**
 * 功能: 单词表中查找对应的单词
 * @param  p     插入找的单词
 * @param  keyno 查找单词的哈希值
 * @return       返回查找到的struct tk_world指针
 */
struct tk_world *
tkworld_find(char *p, int keyno)
{
        struct tk_world *temp = NULL;
        struct list_head *pos;

        // 找到对应的链表进行遍历查询  返回对应结构体的指针
        list_for_each(pos, &(tk_hashtable[keyno])) {
                temp = list_entry(pos, struct tk_world, node);
                // printf("temp->spelling = %s\n", temp->spelling);
                if (strcmp(temp->spelling, p) == 0)
                        return temp;
        }
        return NULL;
}

/**
 * 功能: 标识符插入单词表
 * @param  p 标识符
 */
struct tk_world *
tkworld_insert(char *p)
{
        // printf("p = %s\n", p);
        int keyno = elf_hash(p);
        struct tk_world *tp = tkworld_find(p, keyno);
        if (tp)
                return tp;
        // if (strcmp("point", p) == 0)
        //         printf("\nindex = %d\n", tktable.cur);
        // 进行插入
        int len = strlen(p) + 1;
        tp = malloc(sizeof(struct tk_world));
        tp->spelling = malloc(sizeof(char) * len);
        strcpy(tp->spelling, p);
        tp->etc_code = ++tkcode - 1;

        // printf("add str = %s, etc_code = %d\n", tp->spelling, tp->etc_code);

        struct list_head *insert_head = &(tk_hashtable[keyno]);

        array_add_point(&tktable, tp);

        list_add_tail(&(tp->node), insert_head);
        return tp;
}

/**
 * 功能: 词法分析初始化
 */
void
init_lex()
{
        struct tk_world *tp = NULL;

        for (int i = 0; i < MAXKEY; ++i) {
                INIT_LIST_HEAD(&(tk_hashtable[i]));
        }
        string_init_len(&tkstr, 20);
        string_init_len(&sourcestr, 20);
        point_arr_init(&tktable, MAXKEY);
        tkcode = 0;
        token = 0;

        static struct tk_world keywords[] = {
                { {NULL, NULL}, ETC_PLUS,        NULL, "+",          NULL, NULL },
                { {NULL, NULL}, ETC_MINUS,       NULL, "-",          NULL, NULL },
                { {NULL, NULL}, ETC_STAR,        NULL, "*",          NULL, NULL },
                { {NULL, NULL}, ETC_DIVIDE,      NULL, "/",          NULL, NULL },
                { {NULL, NULL}, ETC_MOD,         NULL, "%",          NULL, NULL },
                { {NULL, NULL}, ETC_EQ,          NULL, "==",         NULL, NULL },
                { {NULL, NULL}, ETC_NEQ,         NULL, "!=",         NULL, NULL },
                { {NULL, NULL}, ETC_LT,          NULL, "<",          NULL, NULL },
                { {NULL, NULL}, ETC_LEQ,         NULL, "<=",         NULL, NULL },
                { {NULL, NULL}, ETC_GT,          NULL, ">",          NULL, NULL },
                { {NULL, NULL}, ETC_GEQ,         NULL, ">=",         NULL, NULL },
                { {NULL, NULL}, ETC_ASSIGN,      NULL, "=",          NULL, NULL },
                { {NULL, NULL}, ETC_POINTTO,     NULL, "->",         NULL, NULL },
                { {NULL, NULL}, ETC_DOT,         NULL, ".",          NULL, NULL },
                { {NULL, NULL}, ETC_AND,         NULL, "&",          NULL, NULL },
                { {NULL, NULL}, ETC_OPENPA,      NULL, "(",          NULL, NULL },
                { {NULL, NULL}, ETC_CLOSEPA,     NULL, ")",          NULL, NULL },
                { {NULL, NULL}, ETC_OPENBR,      NULL, "[",          NULL, NULL },
                { {NULL, NULL}, ETC_CLOSEBR,     NULL, "]",          NULL, NULL },
                { {NULL, NULL}, ETC_OPENBRA,     NULL, "{",          NULL, NULL },
                { {NULL, NULL}, ETC_CLOSEBRA,    NULL, "}",          NULL, NULL },
                { {NULL, NULL}, ETC_SEMCOLON,    NULL, ";",          NULL, NULL },
                { {NULL, NULL}, ETC_COMMA,       NULL, ",",          NULL, NULL },
                { {NULL, NULL}, ETC_ELLPSIS,     NULL, "...",        NULL, NULL },
                { {NULL, NULL}, ETC_EOF,         NULL, "End_Of_File",NULL, NULL },
                { {NULL, NULL}, ETC_CINT,        NULL, "整型常量",    NULL, NULL },
                { {NULL, NULL}, ETC_CCHAR,       NULL, "字符常量",    NULL, NULL },
                { {NULL, NULL}, ETC_CSTR,        NULL, "字符串常量",   NULL, NULL },
                { {NULL, NULL}, ETC_KW_CHAR,     NULL, "char",       NULL, NULL },
                { {NULL, NULL}, ETC_KW_SHORT,    NULL, "short",      NULL, NULL },
                { {NULL, NULL}, ETC_KW_INT,      NULL, "int",        NULL, NULL },
                { {NULL, NULL}, ETC_KW_VOID,     NULL, "void",       NULL, NULL },
                { {NULL, NULL}, ETC_KW_STRUCT,   NULL, "struct",     NULL, NULL },
                { {NULL, NULL}, ETC_KW_IF,       NULL, "if",         NULL, NULL },
                { {NULL, NULL}, ETC_KW_ELSE,     NULL, "else",       NULL, NULL },
                { {NULL, NULL}, ETC_KW_FOR,      NULL, "for",        NULL, NULL },
                { {NULL, NULL}, ETC_KW_CONTINUE, NULL, "continue",   NULL, NULL },
                { {NULL, NULL}, ETC_KW_BREAK,    NULL, "break",      NULL, NULL },
                { {NULL, NULL}, ETC_KW_RETURN,   NULL, "return",     NULL, NULL },
                { {NULL, NULL}, ETC_KW_SIZEOF,   NULL, "sizeof",     NULL, NULL },
                { {NULL, NULL}, ETC_KW_ALIGN,    NULL, "__align",    NULL, NULL },
                { {NULL, NULL}, ETC_KW_CDECL,    NULL, "__cdecl",    NULL, NULL },
                { {NULL, NULL}, ETC_KW_STDCALL,  NULL, "__stdcall",  NULL, NULL },
                { {NULL, NULL}, 0,               NULL, NULL,         NULL, NULL }
        };

        for (tp = &(keywords[0]); tp->spelling != NULL; tp++) {
                tk_word_direct_insert(tp);
                tkcode++;
        }

}
