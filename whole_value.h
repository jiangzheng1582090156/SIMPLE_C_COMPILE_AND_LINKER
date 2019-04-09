#ifndef __WHOLE_VALUE_H__
#define __WHOLE_VALUE_H__

#define MAXKEY 1024

#include "string/common_string.h"
#include "list/list.h"
#include "stack/kstack.h"
#include "list/point_array.h"
#include "semantic_analysis/symbol_table.h"

struct list_head tk_hashtable[MAXKEY];  // 单词哈希表
struct point_array tktable;             // 单词表
int tkcode;                             // 记录单词位置
struct string_t tkstr;                  // 单词字符串
struct string_t sourcestr;              // 单词源码字符串
int tkvalue;                            // 单词的值,单词是整形常量
int ch;                                 // 但钱提取到的源码字符
int token;                              // 单词编码
int line_num;                           // 行号
FILE *fin;

char* filename;                         // 编译的文件名

// 
// STACK_BASE(global_sym_stack);           // 全局符号栈
// STACK_BASE(local_sym_stack);            // 局部符号栈

struct stack_base global_sym_stack;
struct stack_base local_sym_stack;

struct type char_pointer_type;          // 字符串指针类型
struct type int_type;                   // 整数类型
struct type default_func_type;          // 缺省函数类型




#endif // __WHOLE_VALUE_H__
