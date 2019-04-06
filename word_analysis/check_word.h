#ifndef __CHECK_ERROR_H__
#define __CHECK_ERROR_H__

enum e_lex_stage{
        LEX_NORMAL,
        LEX_SEP
};


/**
 * 获得单词v的源码字符串
 * @param  v 单词编号
 * @return   返回字符串
 */
char *get_tkstr(int v);
/**
 * 跳过单词c
 * @param c 单词编号
 */
void skip(int c);
void init();
void cleanup();

/**
 * 从源文件读取一个字节
 */
void getch();

/**
 * 判断是否为字母
 * @param  ch 字符值
 * @return    结果
 */
int is_nodigit(char ch);

/**
 * 判断是否为数字
 * @param  ch 字符值
 * @return    结果
 */
int is_digit(char ch);

/**
 * 解析注释
 */
void parse_commit();

/**
 * 解析标识符
 */
void parse_identifier();

/**
 * 解析数字
 */
void parse_num();

/**
 * 解析字符串
 * @param  sep 字符或者字符串的标识符' ' ' || ' " '
 */
void parse_string(char sep);

/**
 * 跳过空白字符
 */
void skip_white_space();

/**
 * 预处理
 */
void preprocess();

/**
 * 去单词
 */
void get_token();

/**
 * 词法显示和着色
 * @param lex_stage 词法状态枚举
 */
void coloe_token(int lex_stage);


#endif // __CHECK_ERROR_H__
