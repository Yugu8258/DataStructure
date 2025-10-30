#include "DynamicArray.h"

#include <stdio.h>
#include <stdlib.h>

#define INITIAL_CAPACITY 10

// 內部輔助函數: 實現動態數組的擴縮容
static int resize_array(DynamicArray *arr, size_t new_capacity)
{
    // realloc 重新分配內存容量 (擴容)
    // 1. 最佳情況, 原地擴容: 地址沒變, 原先的舊地址旁邊繼續追加空白地址
    // 2. 普遍情況, 移址擴容: 地址變化, 原先的數據全部遷移到新的地址, 並且,
    // 清空原先舊地址的數據
    Data *new_data = (Data *)realloc(arr->data, new_capacity * sizeof(Data));
    if (!new_data)
    {
        // realloc 失敗 內存不足
        fprintf(stderr, "錯誤: 擴容失敗!\n");
        // 這裏返回, 意味着, 原先的那個 arr->data 還在!
        return EXIT_FAILURE;
    }

    // realloc 成功之後, 纔會用新的地址區更新結構體
    arr->data = new_data;
    arr->capacity = new_capacity;

    return EXIT_SUCCESS;
}

DynamicArray *create_array(size_t initial_capacity)
{
    if (initial_capacity <= 0)
    {
        initial_capacity = INITIAL_CAPACITY;
    }

    DynamicArray *array = (DynamicArray *)malloc(sizeof(DynamicArray));
    if (!array)
    {
        fprintf(stderr, "錯誤: 內存分配失敗!\n");
        return NULL;
    }

    // array 作爲結構體變量, 它有三個成員, 這三個成員中, 最重要的就是Data *data;
    // data 指向一個新的連續不斷的內存空間
    // 而整個空間, 現在被我們指向的是一個capacity = initial_capacity的數組
    array->data = (Data *)malloc(sizeof(Data) * initial_capacity);
    if (!array->data)
    {
        free(array);
        return NULL;
    }

    array->size = 0;
    array->capacity = initial_capacity;

    return array;
}

void destory_array(DynamicArray *arr)
{
    if (arr)
    {
        free(arr->data);
        free(arr);
    }
}

void array_append(DynamicArray *arr, Data value)
{
    // 檢查是否需要擴容
    if (arr->size >= arr->capacity)
    {
        size_t new_capacity = arr->capacity << 1;
        resize_array(arr, new_capacity);
    }

    arr->data[arr->size] = value;
    arr->size++;
}

Data *array_read(DynamicArray *arr, size_t index)
{
    if (index >= arr->size)
    {
        puts("數組下標越界!");
        return NULL;
    }

    // address = base_address + index * sizeof(element);
    return &(arr->data[index]);
}

int array_update(DynamicArray *arr, size_t index, Data value)
{
    if (index >= arr->size)
    {
        puts("數組下標越界!");
        return EXIT_FAILURE;
    }

    arr->data[index] = value;

    return EXIT_SUCCESS;
}

int array_insert(DynamicArray *arr, size_t index, Data value)
{
    if (index > arr->size)
    {
        // 索引越界, 允許在數組末尾插入, index == size
        return EXIT_FAILURE;
    }

    // 檢查是否需要擴容
    if (arr->size >= arr->capacity)
    {
        // 函數合約 The Function Contract
        if (resize_array(arr, arr->capacity << 1) != EXIT_SUCCESS)
        {
            return EXIT_FAILURE;
        }
    }

    // 元素後移
    for (size_t i = arr->size; i > index; --i)
    {
        arr->data[i] = arr->data[i - 1];
    }

    arr->data[index] = value;

    arr->size++;

    return EXIT_SUCCESS;
}

int array_delete(DynamicArray *arr, size_t index)
{
    if (index >= arr->size)
    {
        return EXIT_FAILURE;
    }

    for (size_t i = index; i < arr->size; ++i)
    {
        arr->data[i] = arr->data[i + 1];
    }

    arr->size--;

    // 觸發縮容機制
    // 觸發條件
    if (arr->size > 0 && arr->size <= arr->capacity / 4 &&
        arr->capacity > INITIAL_CAPACITY)
    {
        size_t new_capacity = arr->capacity >> 1;

        // 保證縮容後的容量任然能夠裝得下所有元素, 並且不會小於初始容量
        if (new_capacity < arr->size)
        {
            new_capacity = arr->size;
        }

        if (new_capacity < INITIAL_CAPACITY)
        {
            new_capacity = INITIAL_CAPACITY;
        }

        printf("\n----> [縮容警告!] Size (%zu) <= Capacity /4 (%zu). "
               "準備縮容至 %zu. \n",
               arr->size, arr->capacity / 4, new_capacity);

        resize_array(arr, new_capacity);
    }

    return EXIT_SUCCESS;
}

void print_array(DynamicArray *arr)
{
    printf("Array (Size: %zu, Capacity: %zu): [ ", arr->size, arr->capacity);
    for (size_t i = 0; i < arr->size; ++i)
    {
        printf("%d ", arr->data[i]);
    }
    printf("]\n");
}

