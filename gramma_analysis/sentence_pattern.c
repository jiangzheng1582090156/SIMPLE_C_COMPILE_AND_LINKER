#include "../enum_code.h"
#include "../whole_value.h"
#include "sentence_pattern.h"
#include "../error/error.h"
#include "../word_analysis/check_word.h"
// 翻译单元:
// <translation_unit>::={<external_declaration>}<ETC_EOF>
void translation_unit()
{
        while (token != ETC_EOF) {
                external_declaration(SC_GLOBAL);
        }
}

// 外部声明:
// <external_declaration>::=<function_defination> | <declaration>
//
// <function_defination>::=<type_specifier><declarator><funcbody>
//
// <declaration>::=<type_specifier><ETC_SEMCOLON> |
//                 <type_specifier><init_declarator_list><ETC_SEMCOLON>
//
// <init_declarator_list>::=<init_declarator>{<ETC_COMMA><init_declarator>}
//
// <init_declarator>::=<declarator>{<ETC_ASSIGN><initialize>}
//
// 转化为: 函数声明 定义
// <external_declaration>::=<type_specifier>(<ETC_SEMCOLON> |  //声明数据区分符
//                                           <declarator><funcbody> |  // 函数的声明
//                                           <declarator>[<ETC_ASSIGN><initialize>] //变量的定义
//                                           { <ETC_COMMA><declarator>[<ETC_ASSIGN><initialize>] }<ETC_SEMCOLON>)  //后面还可以继续定义变量
/**
 * 功能: 解析外部声明
 * l: 存储类型，局部还是全局
 */
void external_declaration(int l)
{
        if (!type_specifier()) {
                except("<类型区分符>");
        }

        if (token == ETC_SEMCOLON) {
                get_token();
                return ;
        }

        while (1) {  //逐个分析声明 或者 函数定义
                declarator();
                if (token == ETC_OPENBRA) {  // 全局位置定义函数
                        if (l == SC_LOCAL) {
                                error("不支持函数嵌套定义");
                        }
                        funcbody();
                        break;
                } else {
                        if (token == ETC_ASSIGN) {
                                get_token();
                                initialize();
                        }

                        if (token == ETC_COMMA) {
                                get_token();
                        } else {
                                skip(ETC_SEMCOLON);
                                break;
                        }
                }
        }
}




// 类型区分符
// <type_specifier>::=<ETC_KW_INT> | <ETC_KW_CHAR> | <ETC_KW_SHORT> | <ETC_KW_VOID | <struct_specifier>
/**
 * 功能: 解析类型区分符
 * 返回值: 是否发现合法的类型区分符
 */
int type_specifier()
{
        int type_found = 0;
        switch(token) {
        case ETC_KW_CHAR:
                type_found = 1;
                get_token();
                break;
        case ETC_KW_SHORT:
                type_found = 1;
                get_token();
                break;
        case ETC_KW_VOID:
                type_found = 1;
                get_token();
                break;
        case ETC_KW_INT:
                type_found = 1;
                get_token();
                break;
        case ETC_KW_STRUCT:
                struct_specifier();
                type_found = 1;
                break;
        default:
                break;
        }
        return type_found;
}


// 结构区分符
// <struct_specifier>::=<ETC_KW_STRUCT><IDENTIFIER><ETC_OPENBRA><struct_declaration_list><ETC_CLOSEBRA> |
//                      <ETC_KW_STRUCT><IDENTIFIER>
void struct_specifier()
{
        int v;
        get_token();  //得到定义名字
        v = token;

        get_token();


        // 关键字不能做变量定义的名字
        if (v < ETC_KW_IDENT)
                except("结构体名字");

        if (token = ETC_OPENBRA) {
                struct_declaration_list();  // 结构体中的成员
        }

}


// 结构体声明符表
// <struct_declaration_list>::=<struct_declaration>{<struct_declaration>}

void struct_declaration_list()
{
        int maxalign, offset;

        get_token();
        while (token != ETC_CLOSEBRA) {
                struct_declaration(&maxalign, &offset);
        }
        skip(ETC_CLOSEBRA);

}

// 结构声明
// <struct_declaration>::=<type_specifier><struct_declarator_list><ETC_SEMCOLON>
// <struct_declarator_list>::=<declarator>{<ETC_COMMA><declarator>}
// 转换后的标识符
// <struct_declaration>::=<type_specifier><declarator>{<ETC_COMMA><declarator>}<ETC_SEMCOLON>

void struct_declaration()
{
        type_specifier();
        while (1) {
                declarator();

                if (token == ETC_SEMCOLON)
                        break;
                skip(ETC_COMMA);
        }
        skip(ETC_SEMCOLON);  //检查是否缺少了某字符
}




// 函数调用约定
// <function_calling_conversion>::=<ETC_KW_CDECL> | <ETC_KW_STDCALL>

void function_calling_conversion(int *fc)
{
        *fc = ETC_KW_CDECL;
        if (token == ETC_KW_CDECL || token == ETC_KW_STDCALL) {
                *fc = token;
                get_token();
        }
}

// 结构体成员对齐
// <struct_member_alignment>::=<ETC_KW_ALIGN><ETC_OPENPA><ETC_CINT><ETC_CLOSEPA>

void struct_member_alignment()
{
        if (token == ETC_KW_ALIGN) {
                get_token();
                skip(ETC_OPENPA);
                if (token == ETC_CINT) {
                        get_token();
                } else {
                        except("整型常量");
                }

                skip(ETC_CLOSEPA);
        }
}



// 声明符
// <declarator>::={<pointer>}[<function_calling_conversion>][<struct_member_alignment>]<direct_declarator>
// <pointer>::=<ETC_STAR>
//
// 等价转换
// <declarator>::={<ETC_STAR>}[<function_calling_conversion>][<struct_member_alignment>]<direct_declarator>

void declarator()
{
        int fc;
        while (token == ETC_STAR) {
                get_token();
        }
        function_calling_conversion(&fc);
        struct_member_alignment();
        direct_declarator();
}



// 直接声明符
// <direct_declarator>::=<IDENTIFIER><direct_declarator_postfix>
void direct_declarator()
{
        if (token >= ETC_KW_IDENT)
                get_token();
        else
                except("标识符");

        direct_declarator_postfix();
}

// 直接声明符后缀
// <direct_declarator_postfix>::={ <ETC_OPENBR><ETC_CINT><ETC_CLOSEBR>
//                               | <ETC_OPENBR><ETC_CLOSEBR>
//                               | <ETC_OPENPA><parameter_type_list><ETC_CLOSEPA>
//                               | <ETC_OPENPA><ETC_CLOSEPA>}
void direct_declarator_postfix()
{
        int n;
        if (token == ETC_OPENPA) {
                parameter_type_list();
        } else if (token == ETC_OPENBR) {
                get_token();
                get_token();
                if (token == ETC_CINT) {
                        n = tkvalue;
                }
                skip(ETC_CLOSEBR);
                direct_declarator_postfix();
        }
}


// 形参类型表
// <parameter_type_list>::=<parameter_list>
//                         | <parameter_list><ETC_COMMA><ETC_ELLPSIS>
// <parameter_list>::=<parameter_declaration> { <ETC_COMMA><parameter_declaration> }
// <parameter_declaration>::=<type_specifier>{<declarator>}
//
// 转化后:
// <parameter_type_list>::= <type_specifier>{<declarator>}{<ETC_COMMA><type_specifier>{<declarator>}}<ETC_COMMA><ETC_ELLPSIS>

void parameter_type_list()
{
        get_token();

        while (token != ETC_CLOSEPA) {
                if (!type_specifier()) {
                        error("无效的标识符");
                }
                declarator();
                if (token == ETC_CLOSEPA) {
                        break;
                }

                skip(ETC_COMMA);
                if (token == ETC_ELLPSIS) {
                        get_token();
                        break;
                }
        }

        skip(ETC_CLOSEPA);
}




// 函数体
// <funcbody>::=<compand_statement>

void funcbody()
{
        compand_statement();
}


// 初值符
// <initialize>::=<assignment_expression>
void initialize()
{
        assignment_expression();
}

// 语句
// <statement>::=<compand_statement>
//                 | <if_statement>
//                 | <return_statement>
//                 | <break_statement>
//                 | <continue_statement>
//                 | <for_statement>
//                 | <expression_statement>

void statement()
{
        switch(token) {
        case ETC_OPENBRA:
                compand_statement();
        break;
        case ETC_KW_IF:
                if_statement();
        break;
        case ETC_KW_RETURN:
                return_statement();
        break;
        case ETC_KW_BREAK:
                break_statement();
        break;
        case ETC_KW_CONTINUE:
                continue_statement();
        break;
        case ETC_KW_FOR:
                for_statement();
        break;
        default:
                expression_statement();
        break;
        }
}



// 复合语句
// <compand_statement>::=<ETC_OPENBRA>{<declaration>}{<statement>}<ETC_CLOSEBRA>

void compand_statement()
{

        get_token();
        while (is_type_specifier(token)) {
                external_declaration(SC_LOCAL);
        }

        while (token != ETC_CLOSEBRA) {
                statement();
        }

        get_token();
}

/**
 * 判断是否是类型区分符
 * @type {int}
 */
int is_type_specifier(int v)
{
        switch(v) {
        case ETC_KW_INT:
        case ETC_KW_CHAR:
        case ETC_KW_SHORT:
        case ETC_KW_VOID:
        case ETC_KW_STRUCT:
        return 1;
        default:
        break;
        }

        return 0;
}


// 表达式语句
// <expression_statement>::=<ETC_SEMCOLON> | <expression><ETC_SEMCOLON>
void expression_statement()
{
        if (token != ETC_SEMCOLON) {
                expression();
        }
        skip(ETC_SEMCOLON);
}

// 选择语句
// <if_statement>::=<ETC_KW_IF><ETC_OPENPA><expression><ETC_CLOSEPA><statement>[<ETC_KW_ELSE><statement>]
void if_statement()
{
        get_token();
        skip(ETC_OPENPA);
        expression();
        skip(ETC_CLOSEPA);

        statement();
        if (token == ETC_KW_ELSE) {
                get_token();
                statement();
        }
}

// 循环语句
// <for_statement>::=<ETC_KW_FOR><ETC_OPENPA><expression_statement><expression_statement><expression><ETC_CLOSEPA><statement>
void for_statement()
{
        get_token();
        skip(ETC_OPENPA);  //检查是否缺少括号

        // 是否为分号
        if (token != ETC_SEMCOLON) {
                expression();
        }
        skip(ETC_SEMCOLON);
        if (token != ETC_SEMCOLON) {
                expression();
        }
        skip(ETC_SEMCOLON);
        if (token != ETC_CLOSEPA) {
                expression();
        }
        skip(ETC_CLOSEPA);
        statement();
}


// 跳转语句
// continue语句
// <continue_statement>::=<ETC_KW_CONTINUE><ETC_SEMCOLON>
void continue_statement()
{
        get_token();
        skip(ETC_SEMCOLON);
}
// break语句
// <break_statement>::=<ETC_KW_BREAK><ETC_SEMCOLON>
void break_statement()
{
        get_token();
        skip(ETC_SEMCOLON);
}


// return 语句
// <return_statement>::=<ETC_KW_RETURN><ETC_SEMCOLON>
//                         | <ETC_KW_RETURN><expression><ETC_SEMCOLON>
void return_statement()
{
        get_token();


        if (token != ETC_SEMCOLON)
                expression();

        skip(ETC_SEMCOLON);
}

// 表达式
// <expression>::=<assignment_expression>{<ETC_COMMA><assignment_expression>}

void expression()
{
        while (1) {
                assignment_expression();
                if (token != ETC_COMMA)
                        break;

                get_token();
        }
}

// 赋值表达式
// <assignment_expression>::=<equality_expression> |
//                                 <unary_expression><ETC_ASSIGN><assignment_expression>
// 非等价变形
// <assignment_expression>::=<equality_expression>{<ETC_ASSIGN><assignment_expression>}
void assignment_expression()
{
        equality_expression();
        if (token == ETC_ASSIGN) {
                get_token();
                assignment_expression();
        }
}

// 相等类表达式
// <equality_expression>::=<relation_expression> {<ETC_EQ><relation_expression> | <ETC_NEQ><relation_expression>}
void equality_expression()
{
        relation_expression();

        while (token == ETC_EQ || token == ETC_NEQ) {
                get_token();
                relation_expression();
        }
}

// 关系表达式
// <relation_expression>::=<additive_expression>{
//         <ETC_LT><additive_expression>
//         | <ETC_GT><additive_expression>
//         | <ETC_LEQ><additive_expression>
//         | <ETC_GEQ><additive_expression>
// }
void relation_expression()
{
        additive_expression();
        while (token == ETC_LT || token == ETC_GT || token == ETC_GEQ || token == ETC_LEQ) {
                get_token();
                additive_expression();
        }
}

// 加减类表达式
// <additive_expression>::=<multiplicative_expression> {
//         <ETC_PLUS><multiplicative_expression>
//         | <ETC_MINUS><multiplicative_expression>
// }

void additive_expression()
{
        multiplicative_expression();
        while (token == ETC_PLUS || token == ETC_MINUS) {
                get_token();
                multiplicative_expression();
        }
}

// 乘除类表达式
// <multiplicative_expression>::=<unary_expression>{
//         <ETC_STAR><unary_expression>
//         | <ETC_DIVIDE><unary_expression>
//         | <ETC_MOD><unary_expression>
// }
void multiplicative_expression()
{
        unary_expression();
        while (token == ETC_STAR || token == ETC_MOD || token == ETC_DIVIDE) {
                get_token();
                unary_expression();
        }
}

// 一元表达式
// <unary_expression>::=<postfix_expression>
//         | <ETC_AND><unary_expression>
//         | <ETC_STAR><unary_expression>
//         | <ETC_PLUS><unary_expression>
//         | <ETC_MINUS><unary_expression>
//         | <sizeof_expression>
void unary_expression()
{
        switch(token) {
        case ETC_AND:
                get_token();
                unary_expression();
                break;
        case ETC_STAR:
                get_token();
                unary_expression();
                break;
        case ETC_PLUS:
                get_token();
                unary_expression();
                break;
        case ETC_MINUS:
                get_token();
                unary_expression();
                break;
        case ETC_KW_SIZEOF:
                sizeof_expression();
                break;
        default:
                postfix_expression();
                break;
        }
}

// sizeof表达式
// <sizeof_expression>::=<ETC_KW_SIZEOF><ETC_OPENPA><type_specifier><ETC_CLOSEPA>

void sizeof_expression()
{
        get_token();
        skip(ETC_OPENPA);
        type_specifier();
        skip(ETC_CLOSEPA);
}

// 后缀表达式
// <postfix_expression>::=<primary_expression> {
//         <ETC_OPENBR><expression><ETC_CLOSEBR>
//         | <ETC_OPENPA><ETC_CLOSEPA>
//         | <ETC_OPENPA><argument_expression_list><ETC_CLOSEPA>
//         | <ETC_DOT><IDENTIFIER>
//         | <ETC_POINTTO><IDENTIFIER>
// }

void postfix_expression()
{
        primary_expression();
        while (1) {
                if (token == ETC_DOT || token == ETC_POINTTO) {
                        get_token();
                        token |= SC_MEMBER;
                        get_token();
                } else if (token == ETC_OPENBR) {
                        get_token();
                        expression();
                        skip(ETC_CLOSEBR);
                } else if (token == ETC_OPENPA) {
                        argument_expression_list();
                } else {
                        break;
                }
        }
}


// 初值表达式
// <primary_expression>::=<IDENTIFIER>
//                         | <ETC_CINT>
//                         | <ETC_CCHAR>
//                         | <ETC_CSTR>
//                         | <ETC_OPENPA><expression><ETC_CLOSEPA>

void primary_expression()
{
        int t;
        switch(token) {
        case ETC_CINT:
        case ETC_CCHAR:
        case ETC_CSTR:
                get_token();
                break;
        case ETC_OPENPA:
                get_token();
                expression();
                skip(ETC_CLOSEPA);
                break;
        default:
                t = token;
                get_token();
                if (t < ETC_KW_IDENT)
                        except("标识符或者常量");
                break;
        }
}

// 实参表达式表
// <argument_expression_list>::=<assignment_expression>{<ETC_COMMA><assignment_expression>}

void argument_expression_list()
{
        get_token();
        if (token != ETC_CLOSEPA){
                for ( ; ; ) {
                        assignment_expression();
                        if (token == ETC_CLOSEPA)
                                break;
                        skip(ETC_COMMA);
                }
        }

        skip(ETC_CLOSEPA);
}
