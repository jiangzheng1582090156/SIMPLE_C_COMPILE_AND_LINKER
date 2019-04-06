#include <assert.h>
#include <malloc.h>
#include <string.h>


#include "common_string.h"
/**
 * [string_init_len 通过长度初始化字符串结构体]
 * @param str   [字符串结构体]
 * @param count [字符串初始化长度]
 */
void
string_init_len(struct string_t *str, int count)
{
        assert(str != NULL);
        assert(count > 0);

        str->data = malloc(sizeof(char) * count);
        str->count = count;
        str->capacity = 0;
}


/**
 * [string_init_len 通过源字符串初始化字符串结构体]
 * @param str   [字符串结构体]
 * @param src   [源字符串]
 */
void
string_init_str(struct string_t *str, const char *src)
{
        assert(str != NULL && src != NULL);
        int len   = strlen(src);
        str->data = malloc(sizeof(char) * len);
        strcpy(str->data, src);
        str->count    = len;
        str->capacity = len;

}

/**
 * [string_free 释放字符串结构体]
 * @param str [传入的字符串结构体]
 */
void
string_free(struct string_t *str)
{
        assert(str != NULL);

        free(str->data);
        str->capacity = 0;
        str->count    = 0;
}

/**
 * [string_set 设置字符串结构体中的字符串]
 * @param str [字符串结构体]
 * @param src [源字符串]
 */
void
string_set(struct string_t *str, const char *src)
{
        assert(str != NULL && src!= NULL);

        strcpy(str->data, src);
        str->capacity = strlen(src);
}

/**
 * [string_reset 重置字符串]
 * @param str [需要充值的字符串结构体指针]
 */
void
string_reset(struct string_t *str)
{
        assert(str);

        memset(str->data, 0, sizeof(char) * str->count);
        str->capacity = 0;
}

/**
 * [string_realloc 重新设置字符串的长度]
 * @param str      [字符串结构体]
 */
void
string_realloc(struct string_t *str)
{
        int new_size = str->count * 2 + 2;
        str->data = realloc(str->data, new_size * sizeof(char));
        str->count = new_size;
}

/**
 * [string_add_ch 字符串追加字符]
 * @param str [被追加的字符串结构体]
 * @param ch  [需要追加的字节]
 */
void
string_add_ch(struct string_t *str, int ch)
{
        assert(str != NULL);

        if (str->capacity >= str->count)
                string_realloc(str);

        str->data[str->capacity++] = (char)ch;
}
