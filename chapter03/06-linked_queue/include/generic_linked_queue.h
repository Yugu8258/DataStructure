/**
 * @file generic_linked_queue.h
 * @brief 通用链式队列（Linked Queue）的ADT接口定义
 * 基于链表实现，支持任意数据类型，动态扩容无需预设容量
 */
#pragma once

#include <stdbool.h> // 提供 bool 类型
#include <stddef.h>  // 提供 size_t 类型

// 不透明类型声明：隐藏内部实现，用户仅通过指针操作
typedef struct LinkedQueue Queue;

/**
 * @brief 创建一个新的链式队列
 * @param element_size 每个元素的大小（字节），例如 sizeof(int)
 * @return 成功返回队列指针，失败（element_size=0 或内存不足）返回 NULL
 */
Queue *queue_create(size_t element_size);

/**
 * @brief 销毁队列并释放所有内存
 * @param p_queue 指向队列指针的双重指针（确保外部指针被置空）
 */
void queue_destroy(Queue **p_queue);

/**
 * @brief 入队操作：将元素添加到队尾
 * @param queue 队列指针（不可为 NULL）
 * @param element_data 指向待入队元素的指针（不可为 NULL）
 * @return 成功返回 true，失败（内存不足或参数无效）返回 false
 */
bool queue_enqueue(Queue *queue, const void *element_data);

/**
 * @brief 出队操作：从队头移除元素
 * @param queue 队列指针（不可为 NULL）
 * @param output_buffer 接收出队元素的缓冲区（不可为 NULL，需提前分配内存）
 * @return 成功返回 true，失败（队列空或参数无效）返回 false
 */
bool queue_dequeue(Queue *queue, void *output_buffer);

/**
 * @brief 查看队头元素（不移除）
 * @param queue 队列指针（不可为 NULL）
 * @param output_buffer 接收队头元素的缓冲区（不可为 NULL）
 * @return 成功返回 true，失败（队列空或参数无效）返回 false
 */
bool queue_peek(const Queue *queue, void *output_buffer);

/**
 * @brief 检查队列是否为空
 * @param queue 队列指针（可为 NULL，视为空队列）
 * @return 队列为空返回 true，否则返回 false
 */
bool queue_is_empty(const Queue *queue);

/**
 * @brief 获取队列中元素的数量
 * @param queue 队列指针（可为 NULL，返回 0）
 * @return 队列中的元素个数
 */
size_t queue_get_size(const Queue *queue);

