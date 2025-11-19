#pragma once

#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief 优先队列存储的元素类型（可按需修改，如float、结构体）
 * @note 优先级规则：值越大，优先级越高（最大堆实现）
 */
typedef int PQElement;

/**
 * @brief 优先队列结构体（基于最大堆实现）
 * 底层用动态数组存储，维持最大堆特性，确保堆顶为优先级最高元素
 */
typedef struct
{
    PQElement *data; // 动态数组：存储队列元素
    size_t size;     // 当前队列中元素数量
    size_t capacity; // 队列的最大容量（动态扩容）
} PriorityQueue;

// -------------------------- 优先队列核心API --------------------------

/**
 * @brief 创建一个指定初始容量的优先队列
 * @param initial_capacity 初始容量（传入0则使用默认值8）
 * @return 成功返回队列指针，失败返回NULL（内存分配失败）
 */
PriorityQueue *pq_create(size_t initial_capacity);

/**
 * @brief 销毁优先队列，释放所有内存（避免内存泄漏）
 * @param pq 双重指针：指向队列指针的地址（确保销毁后外部指针置NULL）
 */
void pq_destroy(PriorityQueue **pq);

/**
 * @brief 向优先队列中插入一个元素（自动按优先级排序）
 * @param pq 队列指针（不可为NULL）
 * @param elem 待插入的元素
 * @return 成功返回0，失败返回-1（内存分配失败/参数无效）
 */
int pq_push(PriorityQueue *pq, PQElement elem);

/**
 * @brief 取出并移除队列中优先级最高的元素（堆顶元素）
 * @param pq 队列指针（不可为NULL）
 * @param p_elem 输出参数：存储取出的高优先级元素（指针不可为NULL）
 * @return 成功返回0，失败返回-1（队列为空/参数无效）
 */
int pq_pop(PriorityQueue *pq, PQElement *p_elem);

/**
 * @brief 查看队列中优先级最高的元素（不移除）
 * @param pq 队列指针（不可为NULL）
 * @param p_elem 输出参数：存储队首高优先级元素（指针不可为NULL）
 * @return 成功返回0，失败返回-1（队列为空/参数无效）
 */
int pq_top(const PriorityQueue *pq, PQElement *p_elem);

/**
 * @brief 清空队列中所有元素（不释放队列结构体和数组内存）
 * @param pq 队列指针（不可为NULL）
 */
void pq_clear(PriorityQueue *pq);

/**
 * @brief 检查优先队列是否为空
 * @param pq 队列指针（不可为NULL）
 * @return 空队列返回true，非空返回false
 */
bool pq_is_empty(const PriorityQueue *pq);

/**
 * @brief 获取优先队列中元素的数量
 * @param pq 队列指针（不可为NULL）
 * @return 元素数量（size_t类型，队列为NULL返回0）
 */
size_t pq_size(const PriorityQueue *pq);

/**
 * @brief 打印优先队列的详细信息（用于调试）
 * @param pq 队列指针（可为NULL）
 */
void pq_print_debug(const PriorityQueue *pq);

