#include "heap.h"

#include <stdio.h>
#include <string.h> // for memcpy

/**
 * @brief 交换两个Item类型的值(内部辅助函数)
 * @param a 指针a (不可为NULL)
 * @param b 指针b (不可为NULL)
 */
static void _swap(Item *a, Item *b)
{
    Item temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief 堆下沉操作 (维护最大堆特性)
 * @param h 堆指针 (不可为NULL)
 * @param index 待下沉节点的索引
 * @note 循环实现 (避免递归栈溢出), 从index节点向下调整, 与子节点比较并交换
 */
static void _heapify_down(Heap *h, size_t index)
{
    if (!h || index >= h->size)
    {
        return;
    }

    while (true)
    {
        size_t left_child = 2 * index + 1;  // 左子节点索引
        size_t right_child = 2 * index + 2; // 右子节点索引
        size_t largest = index;             // 最大值节点索引 (初始为当前节点)

        // 比较左子节点与当前最大值节点
        if (left_child < h->size && h->data[left_child] > h->data[largest])
        {
            largest = left_child;
        }
        // 比较右子节点与当前最大值节点
        if (right_child < h->size && h->data[right_child] > h->data[largest])
        {
            largest = right_child;
        }

        // 若最大值节点不是当前节点，交换并继续下沉
        if (largest != index)
        {
            _swap(&h->data[index], &h->data[largest]);
            index = largest; // 继续向下调整
        }
        else
        {
            break; // 已满足最大堆特性，退出循环
        }
    }
}

/**
 * @brief 堆上浮操作 (维护最大堆特性)
 * @param h 堆指针 (不可为NULL)
 * @param index 待上浮节点的索引
 * @note 循环实现 (避免递归栈溢出), 从index节点向上调整, 与父节点比较并交换
 */
static void _heapify_up(Heap *h, size_t index)
{
    if (!h || index == 0 || index >= h->size)
    {
        return;
    }

    while (index > 0)
    {
        size_t parent = (index - 1) / 2; // 父节点索引 (整数除法向下取整)

        // 若当前节点大于父节点, 交换并继续上浮
        if (h->data[index] > h->data[parent])
        {
            _swap(&h->data[index], &h->data[parent]);
            index = parent; // 继续向上调整
        }
        else
        {
            break; // 已满足最大堆特性, 退出循环
        }
    }
}

/**
 * @brief 堆动态扩容 (容量翻倍)
 * @param h 堆指针 (不可为NULL)
 * @return 成功返回0, 失败返回-1 (内存分配失败, 原有数据不变)
 * @note 扩容失败时不会修改堆的任何状态, 保证数据安全性
 */
static int _heap_resize(Heap *h)
{
    if (!h)
    {
        return -1;
    }

    size_t new_capacity = h->capacity * 2;
    // 避免溢出 (极端情况下capacity过大, 翻倍后超出size_t范围)
    if (new_capacity < h->capacity)
    {
        fprintf(stderr, "Error: Heap capacity overflow\n");
        return -1;
    }

    // 重新分配内存（保留原有数据）
    Item *new_data = (Item *)realloc(h->data, new_capacity * sizeof(Item));
    if (!new_data)
    {
        fprintf(stderr, "Error: Failed to resize heap (realloc failed)\n");
        return -1;
    }

    // 扩容成功，更新堆的容量和数据指针
    h->data = new_data;
    h->capacity = new_capacity;
    printf("[DEBUG] Heap resized: old capacity=%zu -> new capacity=%zu\n",
           h->capacity / 2, h->capacity);

    return 0;
}

// -------------------------- 公共API实现 --------------------------

Heap *heap_create(size_t initial_capacity)
{
    // 初始容量为0时, 使用默认值8
    if (initial_capacity == 0)
    {
        initial_capacity = 8;
    }

    // 分配堆结构体内存
    Heap *h = (Heap *)malloc(sizeof(Heap));
    if (!h)
    {
        fprintf(stderr, "Error: Failed to allocate heap structure\n");
        return NULL;
    }

    // 分配堆数据数组内存
    h->data = (Item *)malloc(initial_capacity * sizeof(Item));
    if (!h->data)
    {
        fprintf(stderr, "Error: Failed to allocate heap data array\n");
        free(h); // 回滚: 释放结构体内存
        return NULL;
    }

    // 初始化堆状态
    h->size = 0;
    h->capacity = initial_capacity;
    printf("[DEBUG] Heap created: capacity=%zu\n", initial_capacity);

    return h;
}

Heap *heap_create_from_array(const Item *arr, size_t arr_len)
{
    if (!arr || arr_len == 0)
    {
        fprintf(stderr, "Error: Invalid input array (NULL or empty)\n");
        return NULL;
    }

    // 创建堆 (容量为数组长度, 避免扩容)
    Heap *h = heap_create(arr_len);
    if (!h)
    {
        return NULL;
    }

    // 复制数组数据到堆中
    memcpy(h->data, arr, arr_len * sizeof(Item));
    h->size = arr_len;

    // 堆化: 从最后一个非叶子节点开始下沉(时间复杂度O(n))
    size_t last_non_leaf = (h->size - 2) / 2; // 最后一个非叶子节点索引
    for (size_t i = last_non_leaf; i != (size_t)-1; i--)
    {
        _heapify_down(h, i);
    }

    printf("[DEBUG] Heap created from array: size=%zu, capacity=%zu\n", h->size,
           h->capacity);
    return h;
}

void heap_destroy(Heap **h)
{
    // 双重指针确保外部指针置NULL, 避免野指针
    if (h && *h)
    {
        free((*h)->data); // 释放数据数组
        (*h)->data = NULL;
        free(*h);  // 释放堆结构体
        *h = NULL; // 外部指针置NULL
        printf("[DEBUG] Heap destroyed successfully\n");
    }
}

void heap_clear(Heap *h)
{
    if (!h)
    {
        return;
    }

    h->size = 0; // 仅重置元素数量, 不释放内存
    printf("[DEBUG] Heap cleared (size=0)\n");
}

int heap_insert(Heap *h, Item value)
{
    // 参数校验
    if (!h)
    {
        fprintf(stderr, "Error: Heap pointer is NULL\n");
        return -1;
    }

    // 堆满时扩容
    if (heap_is_full(h))
    {
        if (_heap_resize(h) != 0)
        {
            return -1; // 扩容失败
        }
    }

    // 1. 新元素插入到数组末尾 (维持完全二叉树结构)
    h->data[h->size] = value;
    // 2. 元素数量+1
    h->size++;
    // 3. 上浮调整, 维持最大堆特性
    _heapify_up(h, h->size - 1);

    printf("[DEBUG] Inserted value: %d (size=%zu)\n", value, h->size);
    return 0;
}

int heap_extract_max(Heap *h, Item *p_max_value)
{
    // 参数校验
    if (!h || !p_max_value)
    {
        fprintf(stderr, "Error: Invalid parameters (NULL pointer)\n");
        return -1;
    }
    if (heap_is_empty(h))
    {
        fprintf(stderr, "Error: Cannot extract from empty heap\n");
        return -1;
    }

    // 1. 堆顶元素为最大值
    *p_max_value = h->data[0];
    // 2. 最后一个元素移到堆顶（维持完全二叉树结构）
    h->data[0] = h->data[h->size - 1];
    // 3. 元素数量-1
    h->size--;
    // 4. 下沉调整，维持最大堆特性
    if (h->size > 0)
    {
        _heapify_down(h, 0);
    }

    printf("[DEBUG] Extracted max value: %d (size=%zu)\n", *p_max_value,
           h->size);

    return 0;
}

int heap_peek_max(const Heap *h, Item *p_peek_value)
{
    // 参数校验
    if (!h || !p_peek_value)
    {
        fprintf(stderr, "Error: Invalid parameters (NULL pointer)\n");
        return -1;
    }
    if (heap_is_empty(h))
    {
        fprintf(stderr, "Error: Cannot peek from empty heap\n");
        return -1;
    }

    // 直接返回堆顶元素 (不修改堆)
    *p_peek_value = h->data[0];
    printf("[DEBUG] Peeked max value: %d\n", *p_peek_value);
    return 0;
}

void heap_sort(Item *arr, size_t arr_len)
{
    if (!arr || arr_len <= 1)
        return; // 空数组或单元素数组无需排序

    // 步骤1: 构建最大堆 (原地堆化)
    size_t last_non_leaf = (arr_len - 2) / 2;
    for (size_t i = last_non_leaf; i != (size_t)-1; i--)
    {
        // 临时构建堆结构体 (复用_heapify_down逻辑)
        Heap temp_heap = {.data = arr, .size = arr_len, .capacity = arr_len};
        _heapify_down(&temp_heap, i);
    }

    // 步骤2：堆排序（升序）
    for (size_t i = arr_len - 1; i > 0; i--)
    {
        // 交换堆顶 (最大值) 与当前未排序部分的末尾
        _swap(&arr[0], &arr[i]);
        // 临时堆: 未排序部分的长度为i
        Heap temp_heap = {.data = arr, .size = i, .capacity = arr_len};
        // 对堆顶元素下沉, 维持最大堆特性
        _heapify_down(&temp_heap, 0);
    }
}

bool heap_is_empty(const Heap *h)
{
    return (h && h->size == 0);
}

bool heap_is_full(const Heap *h)
{
    return (h && h->size == h->capacity);
}

size_t heap_get_size(const Heap *h)
{
    return (h ? h->size : 0);
}

size_t heap_get_capacity(const Heap *h)
{
    return (h ? h->capacity : 0);
}

bool heap_validate(const Heap *h)
{
    if (!h || h->size <= 1)
        return true; // 空堆或单元素堆默认有效

    // 遍历所有非叶子节点, 验证父节点≥子节点
    size_t last_non_leaf = (h->size - 2) / 2;
    for (size_t i = 0; i <= last_non_leaf; i++)
    {
        size_t left = 2 * i + 1;
        size_t right = 2 * i + 2;

        // 验证左子节点
        if (h->data[i] < h->data[left])
        {
            fprintf(stderr,
                    "Invalid heap: parent=%d < left child=%d (index=%zu)\n",
                    h->data[i], h->data[left], i);
            return false;
        }
        // 验证右子节点 (若存在)
        if (right < h->size && h->data[i] < h->data[right])
        {
            fprintf(stderr,
                    "Invalid heap: parent=%d < right child=%d (index=%zu)\n",
                    h->data[i], h->data[right], i);
            return false;
        }
    }

    return true;
}

void heap_print_debug(const Heap *h)
{
    printf("\n[DEBUG] Heap Info:\n");
    if (!h)
    {
        printf("  Heap: NULL\n");
        return;
    }

    printf("  Capacity: %zu\n", h->capacity);
    printf("  Size: %zu\n", h->size);
    printf("  Is Empty: %s\n", heap_is_empty(h) ? "Yes" : "No");
    printf("  Is Full: %s\n", heap_is_full(h) ? "Yes" : "No");
    printf("  Is Valid Max-Heap: %s\n", heap_validate(h) ? "Yes" : "No");

    if (!heap_is_empty(h))
    {
        printf("  Elements: [");
        for (size_t i = 0; i < h->size; i++)
        {
            printf("%d", h->data[i]);
            if (i != h->size - 1)
            {
                printf(", ");
            }
        }
        printf("]\n");
    }
    else
    {
        printf("  Elements: Empty\n");
    }
    printf("----------------------------------------\n");
}

