/**
 * @file generic_deque.h
 * @brief 泛型双端队列（Deque）的ADT接口定义
 * 基于循环数组实现，支持在队头和队尾双向添加/删除元素，适配任意数据类型
 */
#pragma once

#include <stdbool.h> // 提供 bool 类型
#include <stddef.h>  // 提供 size_t 类型

// 不透明类型声明：隐藏内部实现，用户仅通过指针操作
typedef struct Deque Deque;

/**
 * @brief 创建一个双端队列
 * @param capacity 队列的最大容量（元素数量）
 * @param element_size 每个元素的大小（字节），例如 sizeof(int)
 * @return 成功返回队列指针，失败（参数无效或内存不足）返回 NULL
 */
Deque *deque_create(size_t capacity, size_t element_size);

/**
 * @brief 销毁双端队列并释放所有内存
 * @param p_deque 指向队列指针的双重指针（确保外部指针被置空）
 */
void deque_destroy(Deque **p_deque);

/**
 * @brief 在队头添加元素（前插）
 * @param dq 双端队列指针（不可为 NULL）
 * @param element_data 指向待添加元素的指针（不可为 NULL）
 * @return 成功返回 true，失败（队列满或参数无效）返回 false
 */
bool deque_push_front(Deque *dq, const void *element_data);

/**
 * @brief 在队尾添加元素（后插）
 * @param dq 双端队列指针（不可为 NULL）
 * @param element_data 指向待添加元素的指针（不可为 NULL）
 * @return 成功返回 true，失败（队列满或参数无效）返回 false
 */
bool deque_push_back(Deque *dq, const void *element_data);

/**
 * @brief 从队头移除元素（前删）
 * @param dq 双端队列指针（不可为 NULL）
 * @param output_buffer 接收移除元素的缓冲区（不可为 NULL）
 * @return 成功返回 true，失败（队列空或参数无效）返回 false
 */
bool deque_pop_front(Deque *dq, void *output_buffer);

/**
 * @brief 从队尾移除元素（后删）
 * @param dq 双端队列指针（不可为 NULL）
 * @param output_buffer 接收移除元素的缓冲区（不可为 NULL）
 * @return 成功返回 true，失败（队列空或参数无效）返回 false
 */
bool deque_pop_back(Deque *dq, void *output_buffer);

/**
 * @brief 查看队头元素（不移除）
 * @param dq 双端队列指针（不可为 NULL）
 * @param output_buffer 接收队头元素的缓冲区（不可为 NULL）
 * @return 成功返回 true，失败（队列空或参数无效）返回 false
 */
bool deque_peek_front(const Deque *dq, void *output_buffer);

/**
 * @brief 查看队尾元素（不移除）
 * @param dq 双端队列指针（不可为 NULL）
 * @param output_buffer 接收队尾元素的缓冲区（不可为 NULL）
 * @return 成功返回 true，失败（队列空或参数无效）返回 false
 */
bool deque_peek_back(const Deque *dq, void *output_buffer);

/**
 * @brief 检查队列是否为空
 * @param dq 双端队列指针（可为 NULL，视为空队列）
 * @return 队列为空返回 true，否则返回 false
 */
bool deque_is_empty(const Deque *dq);

/**
 * @brief 检查队列是否已满
 * @param dq 双端队列指针（可为 NULL，视为满队列）
 * @return 队列已满返回 true，否则返回 false
 */
bool deque_is_full(const Deque *dq);

/**
 * @brief 获取当前元素数量
 * @param dq 双端队列指针（可为 NULL，返回 0）
 * @return 队列中的元素个数
 */
size_t deque_get_size(const Deque *dq);

