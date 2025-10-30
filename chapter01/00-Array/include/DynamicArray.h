#pragma once

#include <stddef.h>

// 我們這裏簡單的用int, 假設我們都只存儲int類型數據
typedef int Data;

// typedef 是給一個已經存在的數據類型起一個新的名字 (別名)

// 定義我們動態數組的結構體
typedef struct DynamicArray
{
    Data *data; // 指向存儲數據的連續內存塊; 指向一個數組的首地址
    // int *data; // 說白了, 我們可以放很多int類型的數據,
    // 因爲指向的是一個連續不斷的地址空間

    size_t size;

    size_t capacity;
} DynamicArray;

// 公共接口函數聲明

// 創建並初始化一個動態數組
DynamicArray *create_array(size_t initial_capacity);

// 銷燬數組, 並釋放內存
void destory_array(DynamicArray *arr);

// 在數組末尾追加元素 Amortized O(1)
void array_append(DynamicArray *arr, Data value);

// 讀取指定的索引元素
// 返回一個指針, 以便能夠檢查是否成功, 如果索引無效, 返回NULL
Data *array_read(DynamicArray *arr, size_t index);

// 更新指定索引的元素
// 返回0表示成功, 返回-1表示失敗
int array_update(DynamicArray *arr, size_t index, Data value);

// 在指定索引位置插入元素
// 返回0表示成功, 返回-1表示失敗
int array_insert(DynamicArray *arr, size_t index, Data value);

// 刪除指定索引的元素
// 返回0表示成功, 返回-1表示失敗
int array_delete(DynamicArray *arr, size_t index);

// 打印數組
void print_array(DynamicArray *arr);

