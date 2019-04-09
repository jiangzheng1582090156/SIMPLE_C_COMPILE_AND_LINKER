#ifndef __WORD_TABLE_H__
#define __WORD_TABLE_H__

#include "../enum_code.h"
#include "../list/list.h"
#include "../whole_value.h"
#include "../semantic_analysis/symbol_table.h"


struct tk_world {
        struct list_head node;          // 链表节点
        int etc_code;                   // 单词编码
        struct tk_world *next;          // 指向hash冲突的同名词
        char *spelling;                 // 单词字符串
        struct symbol *sym_struct;      // 指向单词表表示的结构定义 struct point point指向point结构定义
        struct symbol *sym_identifier;  // 指向那个单词所表示的标识符 struct point pt 指向pt变量定义
};




/**
* [elf_hash 计算hash值]
* @param  key [字符串]
* @return     [返回计算的keuno]
*/
int elf_hash(char *key);

/**
 * [tk_word_direct_insert 运算符 关键字 常量 直接放入单词表]
 * @param  tp [description]
 * @return    [description]
 */
void tk_word_direct_insert(struct tk_world *tp);

/**
 * 功能: 单词表中查找对应的单词
 * @param  p     插入找的单词
 * @param  keyno 查找单词的哈希值
 * @return       返回查找到的struct tk_world指针
 */
struct tk_world *tkworld_find(char *p, int keyno);


/**
 * 功能: 标识符插入单词表
 * @param  p 标识符
 */
struct tk_world *tkworld_insert(char *p);

/**
 * 功能: 词法分析初始化
 */
void init_lex();


#endif //__WORD_TABLE_H__
