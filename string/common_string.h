#ifndef __COMMON_STRING_H__
#define __COMMON_STRING_H__

#define STR_DEF_LEN     16

/* 可变长字符串 */
struct string_t {
        int     count;
        int     capacity;
        char    *data;
};

/**
 * [string_init_len 通过长度初始化字符串结构体]
 * @param str   [字符串结构体]
 * @param count [字符串初始化长度]
 */
void string_init_len(struct string_t *str, int count);

/**
 * [string_init_len 通过源字符串初始化字符串结构体]
 * @param str   [字符串结构体]
 * @param src   [源字符串]
 */
void string_init_str(struct string_t *str, const char *src);

/**
 * [string_free 释放字符串结构体]
 * @param str [传入的字符串结构体]
 */
void string_free(struct string_t *str);

/**
 * [string_set 设置字符串结构体中的字符串]
 * @param str [字符串结构体]
 * @param src [源字符串]
 */
void string_set(struct string_t *str, const char *src);

/**
 * [string_reset 重置字符串]
 * @param str [需要充值的字符串结构体指针]
 */
void string_reset(struct string_t *str);

/**
 * [string_realloc 重新设置字符串的长度]
 * @param str      [字符串结构体]
 */
void string_realloc(struct string_t *str);

/**
 * [string_add_ch 字符串追加字符]
 * @param str [被追加的字符串结构体]
 * @param ch  [需要追加的字节]
 */
void string_add_ch(struct string_t *str, int ch);


#endif //__COMMON_STRING_H__
