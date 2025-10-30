#pragma once
#include <stddef.h>

typedef struct Student
{
    int id;
    char name[50];

} Student;

typedef Student Data;

// typedef是给一个已经存在的数据类型起一个新的名字（别名）

// 定义我们动态数组的结构体

//[{stu1.id =1, stu1.name='Frank'}, {},....]

typedef struct
{
    Data *data; // 指向存储数据的连续内存块; 指向一个数组的首地址
                // int* data;
    // 说白了，我们可以放很多int类型的数据，因为指向的是一个连续不断的地址空间
    size_t size;

    size_t capacity;

} DynamicArray;

// 公共接口函数声明
// 创建并初始化一个动态数组
DynamicArray *create_array(size_t initial_capcity);

// 销毁数组，释放内存
void destroy_array(DynamicArray *arr);

// 在数组末尾追加元素 Amortized O(1)
void array_append(DynamicArray *arr, Data value);

// 读取指定的索引元素
// 返回一个指针，以便能够检查是否成功，如果索引无效，返回NULL
Data *array_read(DynamicArray *arr, size_t index);

// 更新指定索引的元素
// 返回0表示成功，返回-1表示失败
int array_update(DynamicArray *arr, size_t index, Data value);
int array_insert(DynamicArray *arr, size_t index, Data value);

// 删除指定索引的元素
int array_delete(DynamicArray *arr, size_t index);

// 打印數組
void print_array(const DynamicArray *arr, void (*print_func)(const void *data));

