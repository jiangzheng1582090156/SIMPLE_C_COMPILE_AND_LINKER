#ifndef __ERROR_H__
#define __ERROR_H__

#include "../whole_value.h"

#include <stdarg.h>

enum e_error_level{
        LEVEL_WARNING,
        LEVEL_ERROR
};

enum e_work_stage{
        STAGE_COMPILE,
        STAGE_LINK
};

/**
 * 异常处理
 * @param stage 异常阶段
 * @param level 异常级别
 * @param fmt   参数输出格式
 * @param ap    可变参数列表
 */
void handle_exception(int stage, int level, char *fmt, va_list ap);

/**
 * 警告处理
 * @param fmt     格式串
 * @param VARARGS 可变参数列表
 */
void warning(char *fmt, ...);

/**
 * 错误处理
 * @param fmt     格式
 * @param VARARGS 可变参数列表
 */
void error(char *fmt, ...);

/**
 * 提示错误
 * @param msg 信息
 */
void except(char *msg);

/**
 * 跳过单词c
 * @param c 单词编号
 */
void skip(int c);


/**
 * 连接错误
 * @param fmt     格式字符串
 * @param VARARGS 可变参数列表
 */
void link_error(char *fmt, ...);

#endif // __ERROR
