/**
 * @file generic_circular_queue.c
 * @brief 通用循环队列的具体实现
 * 内部通过循环数组实现，队头和队尾指针通过模运算实现"绕回"，高效利用空间
 */
#include "generic_circular_queue.h"

#include <stdlib.h> // 提供 malloc、free
#include <string.h> // 提供 memcpy

// 循环队列的内部结构（用户不可见）
struct CircularQueue
{
    void *data;          // 存储元素的数组（动态分配）
    size_t capacity;     // 队列最大容量（元素数量）
    size_t element_size; // 每个元素的大小（字节）
    size_t size;         // 当前元素数量
    int front;           // 队头索引（指向首个元素）
    int rear;            // 队尾索引（指向空闲位置，下一个元素将存在这里）
};

/**
 * @brief 创建循环队列
 */
Queue *queue_create(size_t capacity, size_t element_size)
{
    // 校验参数有效性：容量和元素大小必须大于0
    if (capacity == 0 || element_size == 0)
    {
        return NULL;
    }

    // 分配队列管理结构
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    if (queue == NULL)
    {
        return NULL; // 内存不足
    }

    // 分配元素存储数组（总大小 = 容量 × 元素大小）
    queue->data = malloc(capacity * element_size);
    if (queue->data == NULL)
    {
        free(queue); // 回滚：释放管理结构
        return NULL; // 内存不足
    }

    // 初始化队列状态
    queue->capacity = capacity;
    queue->element_size = element_size;
    queue->size = 0;
    queue->front = 0; // 初始队头指向索引0
    queue->rear = 0;  // 初始队尾指向索引0

    return queue;
}

/**
 * @brief 销毁队列
 */
void queue_destroy(Queue **p_queue)
{
    // 防御性检查：避免空指针解引用
    if (p_queue == NULL || *p_queue == NULL)
    {
        return;
    }

    // 释放内部数据数组和管理结构
    free((*p_queue)->data);
    free(*p_queue);
    *p_queue = NULL; // 置空外部指针，防止悬垂指针
}

/**
 * @brief 入队操作
 */
bool queue_enqueue(Queue *queue, const void *element_data)
{
    // 校验参数有效性和队列状态
    if (queue == NULL || element_data == NULL || queue_is_full(queue))
    {
        return false;
    }

    // 计算元素在数组中的存储位置（字节偏移量）
    // 例如：rear=2，element_size=4 → 偏移量=8，对应数组索引2的位置
    size_t offset = (size_t)queue->rear * queue->element_size;
    void *target_addr = (char *)queue->data + offset;

    // 拷贝元素数据到数组（支持任意类型）
    memcpy(target_addr, element_data, queue->element_size);

    // 更新队尾指针（模运算实现循环）
    queue->rear = (queue->rear + 1) % (int)queue->capacity;

    // 更新元素数量
    queue->size++;
    return true;
}

/**
 * @brief 出队操作
 */
bool queue_dequeue(Queue *queue, void *output_buffer)
{
    // 校验参数有效性和队列状态
    if (queue == NULL || output_buffer == NULL || queue_is_empty(queue))
    {
        return false;
    }

    // 计算队头元素的存储位置（字节偏移量）
    size_t offset = (size_t)queue->front * queue->element_size;
    void *source_addr = (char *)queue->data + offset;

    // 拷贝队头元素到输出缓冲区
    memcpy(output_buffer, source_addr, queue->element_size);

    // 更新队头指针（模运算实现循环）
    queue->front = (queue->front + 1) % (int)queue->capacity;

    // 更新元素数量
    queue->size--;
    return true;
}

/**
 * @brief 查看队头元素
 */
bool queue_peek(const Queue *queue, void *output_buffer)
{
    // 校验参数有效性和队列状态
    if (queue == NULL || output_buffer == NULL || queue_is_empty(queue))
    {
        return false;
    }

    // 计算队头元素的存储位置
    size_t offset = (size_t)queue->front * queue->element_size;
    void *source_addr = (char *)queue->data + offset;

    // 拷贝队头元素到输出缓冲区（不修改队列结构）
    memcpy(output_buffer, source_addr, queue->element_size);
    return true;
}

/**
 * @brief 检查队列是否为空
 */
bool queue_is_empty(const Queue *queue)
{
    if (queue == NULL)
    {
        return true; // 空指针视为空队列
    }
    return queue->size == 0;
}

/**
 * @brief 检查队列是否已满
 */
bool queue_is_full(const Queue *queue)
{
    if (queue == NULL)
    {
        return true; // 空指针视为满队列（防止错误操作）
    }
    return queue->size == queue->capacity;
}

/**
 * @brief 获取当前元素数量
 */
size_t queue_get_size(const Queue *queue)
{
    if (queue == NULL)
    {
        return 0;
    }
    return queue->size;
}

/**
 * @brief 获取队列容量
 */
size_t queue_get_capacity(const Queue *queue)
{
    if (queue == NULL)
    {
        return 0;
    }
    return queue->capacity;
}

