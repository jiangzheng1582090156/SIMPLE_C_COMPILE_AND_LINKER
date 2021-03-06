#ifndef __SENTENCE_PATTERN_H__
#define __SENTENCE_PATTERN_H__

// 翻译单元:
// <translation_unit>::={<external_declaration>}<ETC_EOF>
void translation_unit();
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
void external_declaration(int l);



// 类型区分符
// <type_specifier>::=<ETC_KW_INT> | <ETC_KW_CHAR> | <ETC_KW_SHORT> | <ETC_KW_VOID | <struct_specifier>
/**
 * 功能: 解析类型区分符
 * 返回值: 是否发现合法的类型区分符
 */
int type_specifier();


// 结构区分符
// <struct_specifier>::=<ETC_KW_STRUCT><IDENTIFIER><ETC_OPENBRA><struct_declaration_list><ETC_CLOSEBRA> |
//                      <ETC_KW_STRUCT><IDENTIFIER>
void struct_specifier();

// 结构体声明符表
// <struct_declaration_list>::=<struct_declaration>{<struct_declaration>}

void struct_declaration_list();

// 结构声明
// <struct_declaration>::=<type_specifier><struct_declarator_list><ETC_SEMCOLON>
// <struct_declarator_list>::=<declarator>{<ETC_COMMA><declarator>}
// 转换后的标识符
// <struct_declaration>::=<type_specifier><declarator>{<ETC_COMMA><declarator>}<ETC_SEMCOLON>

void struct_declaration();



// 函数调用约定
// <function_calling_conversion>::=<ETC_KW_CDECL> | <ETC_KW_STDCALL>

void function_calling_conversion();

// 结构体成员对齐
// <struct_member_alignment>::=<ETC_KW_ALIGN><ETC_OPENPA><ETC_CINT><ETC_CLOSEPA>

void struct_member_alignment();


// 声明符
// <declarator>::={<pointer>}[<function_calling_conversion>][<struct_member_alignment>]<direct_declarator>
// <pointer>::=<ETC_STAR>
//
// 等价转换
// <declarator>::={<ETC_STAR>}[<function_calling_conversion>][<struct_member_alignment>]<direct_declarator>

void declarator();


// 直接声明符
// <direct_declarator>::=<IDENTIFIER><direct_declarator_postfix>
void direct_declarator();

// 直接声明符后缀
// <direct_declarator_postfix>::={ <ETC_OPENBR><ETC_CINT><ETC_CLOSEBR>
//                               | <ETC_OPENBR><ETC_CLOSEBR>
//                               | <ETC_OPENPA><parameter_type_list><ETC_CLOSEPA>
//                               | <ETC_OPENPA><ETC_CLOSEPA>}
void direct_declarator_postfix();
// 形参类型表
// <parameter_type_list>::=<parameter_list>
//                         | <parameter_list><ETC_COMMA><ETC_ELLPSIS>
// <parameter_list>::=<parameter_declaration> { <ETC_COMMA><parameter_declaration> }
// <parameter_declaration>::=<type_specifier>{<declarator>}
//
// 转化后:
// <parameter_type_list>::= <type_specifier>{<declarator>}{<ETC_COMMA><type_specifier>{<declarator>}}<ETC_COMMA><ETC_ELLPSIS>

void parameter_type_list();


// 函数体
// <funcbody>::=<compand_statement>

void funcbody();

// 初值符
// <initialize>::=<assignment_expression>
void initialize();
// 语句
// <statement>::=<compand_statement>
//                 | <if_statement>
//                 | <return_statement>
//                 | <break_statement>
//                 | <continue_statement>
//                 | <for_statement>
//                 | <expression_statement>

void statement();


// 复合语句
// <compand_statement>::=<ETC_OPENBRA>{<declaration>}{<statement>}<ETC_CLOSEBRA>

void compand_statement();

/**
 * 判断是否是类型区分符
 * @type {int}
 */
int is_type_specifier(int v);
// 表达式语句
// <expression_statement>::=<ETC_SEMCOLON> | <expression><ETC_SEMCOLON>
void expression_statement();
// 选择语句
// <if_statement>::=<ETC_KW_IF><ETC_OPENPA><expression><ETC_CLOSEPA><statement>[<ETC_KW_ELSE><statement>]
void if_statement();
// 循环语句
// <for_statement>::=<ETC_KW_FOR><ETC_OPENPA><expression_statement><expression_statement><expression><ETC_CLOSEPA><statement>
void for_statement();

// 表达式
// <expression>::=<assignment_expression>{<ETC_COMMA><assignment_expression>}
void expression();

// 跳转语句
// continue语句
// <continue_statement>::=<ETC_KW_CONTINUE><ETC_SEMCOLON>
void continue_statement();
// break语句
// <break_statement>::=<ETC_KW_BREAK><ETC_SEMCOLON>
void break_statement();
// return 语句
// <return_statement>::=<ETC_KW_RETURN><ETC_SEMCOLON>
//                         | <ETC_KW_RETURN><expression><ETC_SEMCOLON>
void return_statement();

// 表达式
// <expression>::=<assignment_expression>{<ETC_COMMA><assignment_expression>}

void assignment_expression();

// 赋值表达式
// <assignment_expression>::=<equality_expression> |
//                                 <unary_expression><ETC_ASSIGN><assignment_expression>
// 非等价变形
// <assignment_expression>::=<equality_expression>{<ETC_ASSIGN><assignment_expression>}
void assignment_expression();
// 相等类表达式
// <equality_expression>::=<relation_expression> {<ETC_EQ><relation_expression> | <ETC_NEQ><relation_expression>}
void equality_expression();
// 关系表达式
// <relation_expression>::=<additive_expression>{
//         <ETC_LT><additive_expression>
//         | <ETC_GT><additive_expression>
//         | <ETC_LEQ><additive_expression>
//         | <ETC_GEQ><additive_expression>
// }
void relation_expression();

// 加减类表达式
// <additive_expression>::=<multiplicative_expression> {
//         <ETC_PLUS><multiplicative_expression>
//         | <ETC_MINUS><multiplicative_expression>
// }

void additive_expression();

// 乘除类表达式
// <multiplicative_expression>::=<unary_expression>{
//         <ETC_STAR><unary_expression>
//         | <ETC_DIVIDE><unary_expression>
//         | <ETC_MOD><unary_expression>
// }
void multiplicative_expression();
// 一元表达式
// <unary_expression>::=<postfix_expression>
//         | <ETC_AND><unary_expression>
//         | <ETC_STAR><unary_expression>
//         | <ETC_PLUS><unary_expression>
//         | <ETC_MINUS><unary_expression>
//         | <sizeof_expression>
void unary_expression();

// sizeof表达式
// <sizeof_expression>::=<ETC_KW_SIZEOF><ETC_OPENPA><type_specifier><ETC_CLOSEPA>

void sizeof_expression();
// 后缀表达式
// <postfix_expression>::=<primary_expression> {
//         <ETC_OPENBR><expression><ETC_CLOSEBR>
//         | <ETC_OPENPA><ETC_CLOSEPA>
//         | <ETC_OPENPA><argument_expression_list><ETC_CLOSEPA>
//         | <ETC_DOT><IDENTIFIER>
//         | <ETC_POINTTO><IDENTIFIER>
// }

void postfix_expression();


// 初值表达式
// <primary_expression>::=<IDENTIFIER>
//                         | <ETC_CINT>
//                         | <ETC_CCHAR>
//                         | <ETC_CSTR>
//                         | <ETC_OPENPA><expression><ETC_CLOSEPA>

void primary_expression();
// 实参表达式表
// <argument_expression_list>::=<assignment_expression>{<ETC_COMMA><assignment_expression>}

void argument_expression_list();


#endif
