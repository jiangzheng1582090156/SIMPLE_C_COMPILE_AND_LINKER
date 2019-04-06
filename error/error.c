#include "error.h"


#include <stdio.h>
#include <stdlib.h>
#include "../enum_code.h"
/**
 * 异常处理
 * @param stage 异常阶段
 * @param level 异常级别
 * @param fmt   参数输出格式
 * @param ap    可变参数列表
 */
void
handle_exception(int stage, int level, char *fmt, va_list ap)
{
        char buf[BUFSIZ] = { 0 };
        vsprintf(buf, fmt, ap);

        if (stage == STAGE_COMPILE) {
                if (level == LEVEL_WARNING) {
                        printf("%s(第 %d 行)：编译警告：%s\n", filename, line_num, buf);
                } else {
                        printf("%s(第 %d 行)：编译错误：%s\n", filename, line_num, buf);
                        exit(-1);
                }
        } else {
                printf("链接错误: %s\n", buf);
                exit(-1);
        }
}

/**
 * 警告处理
 * @param fmt     格式串
 * @param VARARGS 可变参数列表
 */
void
warning(char *fmt, ...)
{
        va_list ap;
        va_start(ap, fmt);

        handle_exception(STAGE_COMPILE, LEVEL_WARNING, fmt, ap);

        va_end(ap);
}

/**
 * 错误处理
 * @param fmt     格式
 * @param VARARGS 可变参数列表
 */
void
error(char *fmt, ...)
{
        va_list ap;
        va_start(ap, fmt);

        handle_exception(STAGE_COMPILE, LEVEL_ERROR, fmt, ap);

        va_end(ap);
}



/**
 * 提示错误
 * @param msg 信息
 */
void
except(char *msg)
{
        error("缺少%s", msg);
}

/**
 * 连接错误
 * @param fmt     格式字符串
 * @param VARARGS 可变参数列表
 */
void
link_error(char *fmt, ...)
{
        va_list ap;
        va_start(ap, fmt);
        handle_exception(STAGE_LINK, LEVEL_ERROR, fmt, ap);
        va_end(ap);
}
