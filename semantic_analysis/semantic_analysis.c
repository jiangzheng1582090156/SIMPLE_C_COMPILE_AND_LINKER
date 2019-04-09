#include "semantic_analysis.h"
#include "../whole_value.h"
#include "../enum_code.h"
#include "../error/error.h"
#include "../word_analysis/check_word.h"
#include "../error/error.h"
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
        struct type btype, type;
        int v, has_init, r, addr;
        struct symbol *sym;

        if (!type_specifier(&btype)) {
                except("<类型区分符>");
        }

        if (token == ETC_SEMCOLON && btype.t == TYPE_STRUCT) {
                get_token();
                return ;
        }

        while (1) {  //逐个分析声明 或者 函数定义
                type = btype;
                declarator(&type, &v, NULL);

                if (token == ETC_OPENBRA) {  // 全局位置定义函数
                        if (l == SC_LOCAL)
                                error("不支持函数嵌套定义");

                        if ((type.t & TYPE_MASK) != TYPE_FUNC)
                                except("<函数定义>");

                        sym = sym_search(v);
                        if (sym) { // 函数在前面声明 现在定义
                                if ((sym->type.t & TYPE_MASK) != TYPE_FUNC)  //没有声明代表函数不同，重定义
                                        error("%s 函数重定义", get_tkstr(v));
                                sym->type = type;
                        } else {
                                func_sym_push(v, &type);
                        }
                        sym->r = SC_SYM | SC_GLOBAL;
                        funcbody(sym);
                        break;
                } else {
                        if ((type.t & TYPE_MASK) == TYPE_FUNC) {  // 函数声明
                                if (!sym_search(v)) {
                                        sym = sym_push(v, &type, SC_GLOBAL | SC_SYM, 0);
                                }
                        } else {  // 变量声明
                                r = 0;
                                if (!(type.t & TYPE_ARRAY))  // 数组名不能当成左值
                                        r |= SC_LVAL;

                                r |= l;
                                has_init = (token == ETC_ASSIGN);  // 是否初始化

                                if (has_init) {
                                        get_token();
                                        initialize(&type);
                                }
                                sym = var_sym_push(&type, r, v, addr);
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
/**
 * 功能: 解析类型区分符
 * @param  type 输出数据类型
 * @return      是否发现合法的类型区分符
 */
int type_specifier(struct type *type/*out*/)
{
        int t, type_found = 0;
        struct type type1;
        t = 0;

        switch(token) {
        case ETC_KW_CHAR:
                t = TYPE_CHAR;
                type_found = 1;
                get_token();
                break;
        case ETC_KW_SHORT:
                t = TYPE_SHORT;
                type_found = 1;
                get_token();
                break;
        case ETC_KW_VOID:
                t = TYPE_VOID;
                type_found = 1;
                get_token();
                break;
        case ETC_KW_INT:
                t = TYPE_INT;
                type_found = 1;
                get_token();
                break;
        case ETC_KW_STRUCT:
                struct_specifier(&type1);
                type->ref = type1.ref;
                t = TYPE_STRUCT;
                type_found = 1;
                break;
        default:
                break;
        }
        type->t = t;
        return type_found;
}


// 结构区分符
// <struct_specifier>::=<ETC_KW_STRUCT><IDENTIFIER><ETC_OPENBRA><struct_declaration_list><ETC_CLOSEBRA> |
//                      <ETC_KW_STRUCT><IDENTIFIER>
void struct_specifier(struct type *type)
{
        int v;
        struct symbol *s;
        struct type type1;

        get_token();  //得到定义名字
        v = token;

        get_token();


        // 关键字不能做变量定义的名字
        if (v < ETC_KW_IDENT)
                except("结构体名字");

        s = struct_search(v);
        if (!s) {
                type1.t = ETC_KW_STRUCT;
                s = sym_push(v | SC_STRUCT, &type1, 0, -1);
                s->r = 0;
        }
        type->t = TYPE_STRUCT;
        type->ref = s;

        if (token = ETC_OPENBRA) {
                struct_declaration_list(type);  // 结构体中的成员
        }

}


// 结构体声明符表
// <struct_declaration_list>::=<struct_declaration>{<struct_declaration>}

void struct_declaration_list(struct type *type)
{
        int maxalign, offset;
        struct symbol *s, **pps;
        s = type->ref;

        get_token();
        if (s->c != -1)
                error("结构体已定义");
        maxalign = 1;
        pps = &(s->next);
        offset = 0;

        while (token != ETC_CLOSEBRA) {
                struct_declaration(&maxalign, &offset, &pps);
        }

        skip(ETC_CLOSEBRA);
        s->c = calc_align(offset, maxalign);    // 结构体大小
        s->r = maxalign;                        // 结构体对齐

}

/**
 * 计算字节对齐位置
 * @param  n     未对齐前置
 * @param  align 对齐粒度
 * @return       int
 */
int
calc_align(int n, int align)
{
        return ((n + align - 1) & (~(align - 1)));
}


// 结构声明
// <struct_declaration>::=<type_specifier><struct_declarator_list><ETC_SEMCOLON>
// <struct_declarator_list>::=<declarator>{<ETC_COMMA><declarator>}
// 转换后的标识符
// <struct_declaration>::=<type_specifier><declarator>{<ETC_COMMA><declarator>}<ETC_SEMCOLON>
/**
 * 功能: 解析成员结构体声明
 * @param maxalign 成员最大对齐粒度
 * @param offset   偏移量
 * @param ps       结构定义符号
 */
void
struct_declaration(int *maxalign/*in out*/, int *offset/*in out*/,
                        struct symbol ***ps/*out*/)
{
        int size, v, align;
        struct symbol *ss;
        struct type type1, btype;
        int force_align;

        type_specifier(&btype);
        while (1) {
                v = 0;
                type1 = btype;
                declarator(&type1, &v, &force_align);
                size = type_size(&type1, &align);

                if (force_align & ALIGN_SET)
                        align = force_align & ~ALIGN_SET;

                *offset = calc_align(*offset, align);

                if (align > *maxalign)
                        *maxalign = align;
                ss = sym_push(v | SC_MEMBER, &type1, 0, *offset);
                *offset += size;
                **ps = ss;
                *ps = &ss->next;

                if (token == ETC_SEMCOLON)
                        break;
                skip(ETC_COMMA);
        }
        skip(ETC_SEMCOLON);  //检查是否缺少了某字符
}




// 函数调用约定
// <function_calling_conversion>::=<ETC_KW_CDECL> | <ETC_KW_STDCALL>

/**
 * 功能: 解析函数调用约定
 * @param fc 调用约定
 */
void
function_calling_conversion(int *fc)
{
        *fc = ETC_KW_CDECL;
        if (token == ETC_KW_CDECL || token == ETC_KW_STDCALL) {
                *fc = token;
                get_token();
        }
}

// 结构体成员对齐
// <struct_member_alignment>::=<ETC_KW_ALIGN><ETC_OPENPA><ETC_CINT><ETC_CLOSEPA>
/**
 * 功能: 解析结构成员对齐
 * @param force_align 强子对齐粒度
 */
void
struct_member_alignment(int *force_align)
{
        int align = 1;
        if (token == ETC_KW_ALIGN) {
                get_token();
                skip(ETC_OPENPA);
                if (token == ETC_CINT) {
                        get_token();
                        align = tkvalue;
                } else {
                        except("整型常量");
                }
                skip(ETC_CLOSEPA);

                if (align != 1 && align != 2 && align != 4)
                        align = 1;
                align |= ALIGN_SET;
                *force_align = ALIGN_SET;
        } else {
                *force_align = 1;
        }
}



// 声明符
// <declarator>::={<pointer>}[<function_calling_conversion>][<struct_member_alignment>]<direct_declarator>
// <pointer>::=<ETC_STAR>
//
// 等价转换
// <declarator>::={<ETC_STAR>}[<function_calling_conversion>][<struct_member_alignment>]<direct_declarator>
/**
 * 功能: 解析
 * @param type        数据类型
 * @param v           单词编号
 * @param force_align 轻质对齐粒度
 */
void
declarator(struct type *type, int *v, int *force_align)
{
        int fc;
        mk_pointer(type);
        while (token == ETC_STAR) {
                get_token();
        }
        function_calling_conversion(&fc);
        struct_member_alignment(force_align);
        direct_declarator(type, v, fc);
}



// 直接声明符
// <direct_declarator>::=<IDENTIFIER><direct_declarator_postfix>
/**
 * 功能: 解析直接声明符
 * @param type     数据类型
 * @param v        单词编号
 * @param fun_call 函数调用约定
 */
void
direct_declarator(struct type *type, int *v, int fun_call)
{
        if (token >= ETC_KW_IDENT) {
                *v = token;
                get_token();
        } else {
                except("标识符");
        }

        direct_declarator_postfix(type, fun_call);
}

// 直接声明符后缀
// <direct_declarator_postfix>::={ <ETC_OPENBR><ETC_CINT><ETC_CLOSEBR>
//                               | <ETC_OPENBR><ETC_CLOSEBR>
//                               | <ETC_OPENPA><parameter_type_list><ETC_CLOSEPA>
//                               | <ETC_OPENPA><ETC_CLOSEPA>}
/**
 * 功能: 直接声明符后缀
 * @param type      数据类型
 * @param func_call 函数调用约定
 */
void
direct_declarator_postfix(struct type *type, int func_call)
{
        int n;
        struct symbol *s;
        if (token == ETC_OPENPA) {
                parameter_type_list(type, func_call);
        } else if (token == ETC_OPENBR) {
                get_token();
                n = -1;
                if (token == ETC_CINT) {
                        get_token();
                        n = tkvalue;
                }
                skip(ETC_CLOSEBR);
                direct_declarator_postfix(type, func_call);
                s = sym_push(SC_ANOM, type, 0, n);
                type->t = TYPE_ARRAY | TYPE_PTR;
                type->ref = s;
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
/**
 * 功能: 解析形参类型表
 * @param type     数据类型
 * @param fun_call 函数调用约定
 */
void
parameter_type_list(struct type *type, int fun_call)
{
        int n;
        struct symbol **plast, *s, *first;
        struct type pt;

        get_token();
        first = NULL;
        plast = &first;

        while (token != ETC_CLOSEPA) {
                if (!type_specifier(&pt)) {
                        error("无效的标识符");
                }

                declarator(&pt, &n, NULL);
                s = sym_push(n | SC_PARAMS, &pt, 0, 0);
                *plast = s;
                plast = &(s->next);

                if (token == ETC_CLOSEPA) {
                        break;
                }

                skip(ETC_COMMA);
                if (token == ETC_ELLPSIS) {
                        fun_call = ETC_KW_CDECL;
                        get_token();
                        break;
                }
        }

        skip(ETC_CLOSEPA);

        // 将此处函数返回值类型存储，然后指向参数，最后将type设为函数类型，引用的相关信息放在ref中
        s = sym_push(SC_ANOM, type, fun_call, 0);
        s->next = first;
        type->t = TYPE_FUNC;
        type->ref = s;
}




// 函数体
// <funcbody>::=<compand_statement>
/**
 * 功能: 解析函数体
 * @param sym 函数符号
 */
void
funcbody(struct symbol *sym)
{
        // 放匿名符号在局部表中
        __sym_push(&local_sym_stack, SC_ANOM, &int_type, 0);
        compand_statement(NULL, NULL);

        // 清空局部符号栈
        stack_clear(&local_sym_stack);
}


// 初值符
// <initialize>::=<assignment_expression>
/**
 * 功能: 解析初始符
 * @param type 变量类型
 */
void
initialize(struct type *type)
{
        if (type->t & TYPE_ARRAY)
                get_token();
        else
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

void
statement(int *bsym, int *csym)
{
        switch(token) {
        case ETC_OPENBRA:
                compand_statement(bsym, csym);
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
/**
 * 功能: 解析符合语句
 * @param bsym 跳转位置
 * @param csym 跳转位置
 */
void
compand_statement(int *bsym, int *csym)
{
        struct symbol *s;
        s = stack_top_entry(&local_sym_stack, struct symbol, node);

        get_token();
        while (is_type_specifier(token)) {
                external_declaration(SC_LOCAL);
        }

        while (token != ETC_CLOSEBRA) {
                statement(bsym, csym);
        }

        sym_pop(&local_sym_stack, s);

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
void if_statement(int *bsym, int *csym)
{
        get_token();
        skip(ETC_OPENPA);
        expression();
        skip(ETC_CLOSEPA);

        statement(bsym, csym);
        if (token == ETC_KW_ELSE) {
                get_token();
                statement(bsym, csym);
        }
}

// 循环语句
// <for_statement>::=<ETC_KW_FOR><ETC_OPENPA><expression_statement><expression_statement><expression><ETC_CLOSEPA><statement>
void for_statement(int *bsym, int *csym)
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
        statement(bsym, csym);
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
        int align, size;
        struct type type;

        get_token();
        skip(ETC_OPENPA);
        type_specifier(&type);
        skip(ETC_CLOSEPA);

        size = type_size(&type, &align);
        if (size < 0)
                error("sizeof 计算类型大小失败");
}

/**
 * 功能: 返回类型的长度
 * @param  t 数据类型指针
 * @param  a 对齐值
 * @return   返回类型长度
 */
int
type_size(struct type *t, int *a)
{
        struct symbol *s;
        int bt;
        // 指针类型长度
        int PTR_SIZE = 4;
        bt = t->t & TYPE_MASK;

        switch (bt) {
        case TYPE_STRUCT:
                s = t->ref;
                *a = s->r;
                return s->c;
        case TYPE_PTR:
                if (t->t & TYPE_ARRAY) {
                        s = t->ref;
                        return type_size(&s->type, a) * s->c;
                } else {
                        *a = PTR_SIZE;
                        return PTR_SIZE;
                }
        case TYPE_INT:
                *a = 4;
                return 4;
        case TYPE_SHORT:
                *a = 2;
                return 2;
        default:
                *a = 1;
                return 1;
        }
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

/**
 * 功能:	生成指针类型
 * @param t 原数据类型
 */
void mk_pointer(struct type *t)
{
	struct symbol *s;
    s = sym_push(SC_ANOM, t, 0, -1);
    t->t = TYPE_PTR ;
    t->ref = s;
}

// 初值表达式
// <primary_expression>::=<IDENTIFIER>
//                         | <ETC_CINT>
//                         | <ETC_CCHAR>
//                         | <ETC_CSTR>
//                         | <ETC_OPENPA><expression><ETC_CLOSEPA>
/**
 * 功能: 解析初等表达式
 */
void
primary_expression()
{
        int t, addr;
        struct type type;
        struct symbol  *s;

        switch(token) {
        case ETC_CINT:
        case ETC_CCHAR:
                get_token();
                break;
        case ETC_CSTR:
                t = TYPE_CHAR;
                type.t = t;
                mk_pointer(&type);
                type.t |= TYPE_ARRAY;
                var_sym_push(&type, SC_GLOBAL, 0, addr);
                initialize(&type);
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
                s = sym_search(t);
                if (!t) {
                        if (token != ETC_OPENPA)
                                error("'%s' 未声明", get_tkstr(t));

                        s = func_sym_push(t, &default_func_type);
                        s->r = SC_GLOBAL | SC_SYM;
                }

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
