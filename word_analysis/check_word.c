#include "../whole_value.h"

#include "word_table.h"
#include "check_word.h"
#include  "../error/error.h"

#include <stdlib.h>
#include <stdio.h>


/**
 * 获得单词v的源码字符串
 * @param  v 单词编号
 * @return   返回字符串
 */
char *
get_tkstr(int v)
{
        if (v > tkcode)
                return NULL;
        else if (v >= ETC_CINT && v <= ETC_CSTR)
                return sourcestr.data;
        else
                return ((struct tk_world *)tktable.array[v])->spelling;
}


/**
 * 跳过单词c
 * @param c 单词编号
 */
void
skip(int c)
{
        if (token != c) {
                error("缺少%s\n", get_tkstr(c));
        }

        get_token();
}

void
init()
{
        line_num = 1;
        init_lex();
}

void
cleanup()
{

}

/**
 * 从源文件读取一个字节
 */
void
getch()
{
        // ch = getc(fin);
        ch = getc(fin);
}

/**
 * 解析注释
 */
void
parse_commit()
{
        getch();
        do {
                do {
                        if (ch == '\n' || ch == '*' || ch == EOF)
                                break;
                        else
                                getch();
                } while(1);

                if (ch == '\n') {
                        line_num++;
                        getch();
                } else if (ch == '*') {
                        getch();
                        if (ch == '/') {
                                getch();
                                return ;
                        }
                } else {
                        error("一直在文件尾未看到注释符\n");
                        return ;
                }
        } while(1);
}

/**
 * 跳过空白字符
 */
void
skip_white_space()
{
        while (ch == ' ' || ch == '\t' || ch == '\n') {
                if (ch == '\n') {
                        line_num++;
                }
                printf("%c", ch);
                getch();
        }

}

/**
 * 判断是否为字母
 * @param  ch 字符值
 * @return    结果
 */
int
is_nodigit(char ch)
{
        return ((ch >= 'A' && ch <= 'Z') ||
                (ch >= 'a' && ch <= 'z') ||
                (ch == '_'));
}

/**
 * 判断是否为数字
 * @param  ch 字符值
 * @return    结果
 */
int
is_digit(char ch)
{
        return ch >= '0' && ch <= '9';
}

/**
 * 解析标识符
 */
void
parse_identifier()
{
        // printf("start ch = %c ", ch);
        string_reset(&tkstr);
        string_add_ch(&tkstr, ch);
        getch();
        while (is_nodigit(ch) || is_digit(ch)) {
                // printf("%c", ch);
                string_add_ch(&tkstr, ch);
                getch();
        }

        string_add_ch(&tkstr, '\0');

        // printf("str = %s after get ch = %c\n", tkstr.data, ch);

}

/**
 * 解析数字
 */
void
parse_num()
{
        string_reset(&tkstr);
        string_reset(&sourcestr);
        do {
                string_add_ch(&tkstr, ch);
                string_add_ch(&sourcestr, ch);
                getch();
        } while(is_digit(ch));

        if (ch == '.') {
                do {
                        string_add_ch(&tkstr, ch);
                        string_add_ch(&sourcestr, ch);
                        getch();
                } while(is_digit(ch));
        }
        string_add_ch(&tkstr, '\0');
        string_add_ch(&sourcestr, '\0');

        tkvalue = atoi(tkstr.data);
}


/**
 * 解析字符串
 */
void
parse_string(char sep)
{
        char c;
        string_reset(&tkstr);
        string_reset(&sourcestr);
        string_add_ch(&sourcestr, sep);
        getch();

        for (;;) {
                if (ch == sep) {
                        break;
                } else if (ch == '\\') { // 转义字符
                        string_add_ch(&sourcestr, ch);
                        getch();

                        switch (ch) {
                                case '0': c = '\0'; break;
                                case 'b': c = '\b'; break;
                                case 'a': c = '\a'; break;
                                case 't': c = '\t'; break;
                                case 'n': c = '\n'; break;
                                case 'v': c = '\v'; break;
                                case 'f': c = '\f'; break;
                                case 'r': c = '\r'; break;
                                case '\\': c = '\\'; break;
                                case '\"': c = '\"'; break;
                                case '\'': c = '\''; break;
                                default:
                                c = ch;
                                warning("非法转义字符");
                                break;
                        }
                        string_add_ch(&tkstr, c);
                        string_add_ch(&sourcestr, ch);
                        getch();
                } else {
                        string_add_ch(&tkstr, ch);
                        string_add_ch(&sourcestr, ch);
                        getch();
                }
        }

        string_add_ch(&tkstr, '\0');
        string_add_ch(&sourcestr, sep);
        string_add_ch(&sourcestr, '\0');
        getch();

}


/**
 * 预处理
 */
void
preprocess()
{
        while (1) {
                if (ch == ' ' || ch == '\t' || ch == '\n') {
                        skip_white_space();
                } else if (ch == '/') {
                        // 向前多读一个字符 判断是否为注释
                        getch();
                        if (ch == '*') {
                                parse_commit();
                        } else {
                                ungetc(ch, fin);
                                ch = '/';
                                break;
                        }
                } else {
                        break;
                }
        }
}

/**
 * 词法显示和着色
 * @param lex_stage 词法状态枚举
 */
void
coloe_token(int lex_stage)
{
        char *p = get_tkstr(token);
        // 标识符
        if (token >= ETC_KW_IDENT) {
                printf("\e[37;1m%s\e[0m", p);
                return ;
        } else if (token >= ETC_KW_CHAR) {
                // 关键字
                printf("\e[34;1m%s\e[0m", p);
                return ;
        } else if (token >= ETC_CINT) {
                // 常量
                printf("\e[33;1m%s\e[0m", p);
                return ;
        }
        // 运算符
        printf("\e[31;1m%s\e[0m", p);
}


/**
 * 去单词
 */
void
get_token()
{
        preprocess();

        switch (ch) {
                case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':case 'g':
                case 'h':case 'i':case 'j':case 'k':case 'l':case 'm':case 'n':
                case 'o':case 'p':case 'q':case 'r':case 's':case 't':
                case 'u':case 'v':case 'w':case 'x':case 'y':case 'z':
                case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':case 'G':
                case 'H':case 'I':case 'J':case 'K':case 'L':case 'M':case 'N':
                case 'O':case 'P':case 'Q':case 'R':case 'S':case 'T':
                case 'U':case 'V':case 'W':case 'X':case 'Y':case 'Z':
                case '_':
                {
                        // printf("ch = %c ", ch);
                        struct tk_world *tp;
                        // 解析标识符
                        parse_identifier();
                        tp = tkworld_insert(tkstr.data);
                        token = tp->etc_code;

                        // printf("ch = %c ", ch);
                        // printf("tkstr = %s\n", tkstr.data);
                        // printf("tkcode = %d\n", tp->etc_code);


                        break;
                }
                case '0':case '1':case '2':case '3':case '4':case '5':case '6':
                case '7':case '8':case '9':
                {
                        parse_num();
                        token = ETC_CINT;
                        break;
                }
                case '+':
                {
                        getch();
                        token = ETC_PLUS;
                        break;
                }
                case '-':
                {
                        getch();
                        if (ch == '>') {
                                token = ETC_POINTTO;
                                getch();
                        } else {
                                token = ETC_MINUS;
                        }
                        break;
                }
                case '/':
                {
                        token = ETC_DIVIDE;
                        getch();
                        break;
                }
                case '%':
                {
                        token = ETC_MOD;
                        getch();
                        break;
                }
                case '=':
                {
                        getch();
                        if (ch == '=') {
                                token = ETC_EQ;
                                getch();
                        } else {
                                token = ETC_ASSIGN;
                        }
                        break;
                }
                case '!':
                {
                        getch();
                        if (ch == '=') {
                                token = ETC_NEQ;
                                getch();
                        } else {
                                error("暂不支持该操作符");
                        }
                        break;
                }
                case '<':
                {
                        getch();
                        if (ch == '=') {
                                token = ETC_LEQ;
                                getch();
                        } else {
                                token = ETC_LT;
                        }
                        break;
                }
                case '>':
                {
                        getch();
                        if (ch == '=') {
                                token = ETC_GEQ;
                                getch();
                        } else {
                                token = ETC_GT;
                        }
                        break;
                }
                case '.':
                {
                        getch();
                        if (ch == '.') {
                                getch();
                                if (ch != '.')
                                        error("省略号拼写错误");
                                else
                                        token = ETC_ELLPSIS;
                                getch();
                        } else {
                                token = ETC_DOT;
                        }
                }
                case '&':
                {
                        token = ETC_AND;
                        getch();
                        break;
                }
                case ';':
                {
                        token = ETC_SEMCOLON;
                        getch();
                        break;
                }
                case ']':
                {
                        token = ETC_CLOSEBR;
                        getch();
                        break;
                }
                case '}':
                {
                        token = ETC_CLOSEBRA;
                        getch();
                        break;
                }
                case ')':
                {
                        token = ETC_CLOSEPA;
                        getch();
                        break;
                }
                case '[':
                {
                        token = ETC_OPENBR;
                        getch();
                        break;
                }
                case '{':
                {
                        token = ETC_OPENBRA;
                        getch();
                        break;
                }
                case '(':
                {
                        token = ETC_OPENPA;
                        getch();
                        break;
                }
                case ',':
                {
                        token = ETC_COMMA;
                        getch();
                        break;
                }
                case '*':
                {
                        token = ETC_STAR;
                        getch();
                        break;
                }
                case '\'':
                {
                        // 解析字符
                        parse_string(ch);
                        token = ETC_CCHAR;
                        tkvalue = * (char *)tkstr.data;
                        break;
                }
                case '\"':
                {
                        parse_string(ch);
                        token = ETC_CSTR;
                        break;
                }
                case EOF:
                {
                        token = ETC_EOF;
                        break;
                }
                default:
                {
                        error("不认识的字符: \\x%02x", ch);
                        getch();
                        break;
                }
        }

        // syntax_indent();
        coloe_token(LEX_NORMAL);
}
