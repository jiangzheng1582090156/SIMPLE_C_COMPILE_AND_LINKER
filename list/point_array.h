#ifndef __POINT_ARRAY_H__
#define __POINT_ARRAY_H__

#include <malloc.h>

struct point_array{
        unsigned int size;
        unsigned int cur;
        void **array;
};

/**
 * 添加指针
 * @param array 指针数组结构体
 * @param size  指针数组长度
 */
static void
point_arr_init(struct point_array *array, unsigned int size)
{
        array->array = malloc(sizeof(void *) * size);
        array->size = size;
        array->cur = 0;
}

/**
 * 添加指针
 * @param  array 指针数组结构体指针
 * @param  p     添加的指针
 * @return       void
 */
static void
array_add_point(struct point_array *array, void *p)
{
        if (array->cur >= array->size)
                return ;

        array->array[array->cur++] = p;
}

#endif //__POINT_ARRAY_H__
