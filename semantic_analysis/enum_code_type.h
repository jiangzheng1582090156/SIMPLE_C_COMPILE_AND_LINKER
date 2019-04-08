#ifndef __ENUM_CODE_TYPE_H__
#define __ENUM_CODE_TYPE_H__

enum e_code_type {
        TYPE_INT        = 0,
        TYPE_CHAR       = 1,
        TYPE_SHORT      = 2,
        TYPE_VOID       = 3,
        TYPE_PTR        = 4,
        TYPE_FUNC       = 5,
        TYPE_STRUCT     = 6,

        TYPE_MASK       = 0x000f;  //基本数据类型掩码
        TYPE_ARRAY      = 0x0010;  //数组
};

#endif //__ENUME_CODE_TYPE_H__
