// heap.h
#pragma once

#pragma once

#include <stdbool.h> // for bool type
#include <stdlib.h>  // for size_t

/**
 * @brief 堆存储的数据类型(可按需修改, 如改为float、自定义结构体)
 * @note 若修改为结构体, 需同步修改 swap、heapify_up、heapify_down 中的比较逻辑
 */
typedef int Item;

/**
 * @brief 最大堆结构体定义
 * 最大堆特性: 父节点值 ≥ 左、右子节点值，完全二叉树结构(用动态数组存储)
 */
typedef struct
{
    Item *data;      // 动态数组: 存储堆元素(索引从0开始)
    size_t size;     // 当前堆中元素数量(有效元素个数)
    size_t capacity; // 堆的最大容量(动态数组当前可容纳的元素个数)
} Heap;

// -------------------------- 公共API(按功能分类)--------------------------

/**
 * @brief 创建一个指定初始容量的最大堆
 * @param initial_capacity 初始容量(若传入0, 使用默认值8)
 * @return 成功返回堆指针, 失败返回NULL(内存分配失败)
 */
Heap *heap_create(size_t initial_capacity);

/**
 * @brief 从数组快速构建最大堆(堆化操作, 时间复杂度O(n))
 * @param arr 输入数组(元素类型为Item)
 * @param arr_len 数组长度
 * @return 成功返回堆指针, 失败返回NULL(内存分配失败)
 */
Heap *heap_create_from_array(const Item *arr, size_t arr_len);

/**
 * @brief 销毁堆, 释放所有内存(避免内存泄漏)
 * @param h 双重指针: 指向堆指针的地址(确保销毁后外部指针置NULL)
 */
void heap_destroy(Heap **h);

/**
 * @brief 清空堆中所有元素(不释放堆结构体和数组内存, 仅重置状态)
 * @param h 堆指针(不可为NULL)
 */
void heap_clear(Heap *h);

/**
 * @brief 向堆中插入一个元素(维持最大堆特性)
 * @param h 堆指针(不可为NULL)
 * @param value 待插入的元素值
 * @return 成功返回0, 失败返回-1(内存分配失败/参数无效)
 */
int heap_insert(Heap *h, Item value);

/**
 * @brief 提取堆顶最大值(并从堆中移除)
 * @param h 堆指针(不可为NULL)
 * @param p_max_value 输出参数: 存储提取的最大值(指针不可为NULL)
 * @return 成功返回0, 失败返回-1(堆为空/参数无效)
 */
int heap_extract_max(Heap *h, Item *p_max_value);

/**
 * @brief 查看堆顶最大值(不移除元素)
 * @param h 堆指针(不可为NULL)
 * @param p_peek_value 输出参数: 存储堆顶值(指针不可为NULL)
 * @return 成功返回0, 失败返回-1(堆为空/参数无效)
 */
int heap_peek_max(const Heap *h, Item *p_peek_value);

/**
 * @brief 堆排序(升序): 将输入数组按最大堆排序
 * @param arr 输入输出数组(排序后为升序)
 * @param arr_len 数组长度
 * @note 直接修改原数组, 时间复杂度O(nlogn), 空间复杂度O(1)(原地排序)
 */
void heap_sort(Item *arr, size_t arr_len);

/**
 * @brief 检查堆是否为空
 * @param h 堆指针(不可为NULL)
 * @return 空堆返回true, 非空返回false
 */
bool heap_is_empty(const Heap *h);

/**
 * @brief 检查堆是否已满
 * @param h 堆指针(不可为NULL)
 * @return 满堆返回true, 未满返回false
 */
bool heap_is_full(const Heap *h);

/**
 * @brief 获取堆中元素数量
 * @param h 堆指针(不可为NULL)
 * @return 元素数量(size_t类型, 堆为NULL返回0)
 */
size_t heap_get_size(const Heap *h);

/**
 * @brief 获取堆的当前容量
 * @param h 堆指针(不可为NULL)
 * @return 堆容量(size_t类型, 堆为NULL返回0)
 */
size_t heap_get_capacity(const Heap *h);

/**
 * @brief 验证堆是否符合最大堆特性(用于调试)
 * @param h 堆指针(不可为NULL)
 * @return 符合返回true, 不符合返回false
 */
bool heap_validate(const Heap *h);

/**
 * @brief 打印堆的详细信息(用于调试)
 * @param h 堆指针(可为NULL)
 */
void heap_print_debug(const Heap *h);

// --------------------------
// 内部辅助函数声明(仅实现文件可见)--------------------------

static void _swap(Item *a, Item *b);

static void _heapify_down(Heap *h, size_t index);

static void _heapify_up(Heap *h, size_t index);

static int _heap_resize(Heap *h);

