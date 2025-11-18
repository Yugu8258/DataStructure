/**
 * @file generic_deque.c
 * @brief 泛型双端队列的具体实现
 * 基于循环数组实现，通过 front 和 rear 指针的双向移动支持双端操作
 */
#include "generic_deque.h"

#include <stdlib.h> // 提供 malloc、free
#include <string.h> // 提供 memcpy

// 双端队列的内部结构（用户不可见）
struct Deque
{
    void *data;          // 存储元素的循环数组（动态分配）
    size_t capacity;     // 最大容量（元素数量）
    size_t element_size; // 每个元素的大小（字节）
    size_t size;         // 当前元素数量
    int front;           // 队头索引（指向首个元素）
    int rear;            // 队尾索引（指向最后一个元素的下一个位置）
};

/**
 * @brief 创建双端队列
 */
Deque *deque_create(size_t capacity, size_t element_size)
{
    // 校验参数有效性：容量和元素大小必须大于0
    if (capacity == 0 || element_size == 0)
    {
        return NULL;
    }

    // 分配队列管理结构
    Deque *dq = (Deque *)malloc(sizeof(Deque));
    if (dq == NULL)
    {
        return NULL; // 内存不足
    }

    // 分配元素存储数组（总大小 = 容量 × 元素大小）
    dq->data = malloc(capacity * element_size);
    if (dq->data == NULL)
    {
        free(dq);    // 回滚：释放管理结构
        return NULL; // 内存不足
    }

    // 初始化队列状态
    dq->capacity = capacity;
    dq->element_size = element_size;
    dq->size = 0;
    dq->front = 0; // 初始队头指向索引0
    dq->rear = 0;  // 初始队尾指向索引0（队列为空时 front == rear）

    return dq;
}

/**
 * @brief 销毁双端队列
 */
void deque_destroy(Deque **p_deque)
{
    // 防御性检查：避免空指针解引用
    if (p_deque == NULL || *p_deque == NULL)
    {
        return;
    }

    // 释放内部数据数组和管理结构
    free((*p_deque)->data);
    free(*p_deque);
    *p_deque = NULL; // 置空外部指针，防止悬垂指针
}

/**
 * @brief 检查队列是否为空
 */
bool deque_is_empty(const Deque *dq)
{
    if (dq == NULL)
    {
        return true; // 空指针视为空队列
    }
    return dq->size == 0;
}

/**
 * @brief 检查队列是否已满
 */
bool deque_is_full(const Deque *dq)
{
    if (dq == NULL)
    {
        return true; // 空指针视为满队列
    }
    return dq->size == dq->capacity;
}

/**
 * @brief 获取当前元素数量
 */
size_t deque_get_size(const Deque *dq)
{
    if (dq == NULL)
    {
        return 0;
    }
    return dq->size;
}

/**
 * @brief 在队尾添加元素（后插）
 * 与循环队列的入队操作逻辑相同
 */
bool deque_push_back(Deque *dq, const void *element_data)
{
    // 校验参数有效性和队列状态
    if (dq == NULL || element_data == NULL || deque_is_full(dq))
    {
        return false;
    }

    // 计算插入位置（rear 指向的空闲位置）
    size_t offset = (size_t)dq->rear * dq->element_size;
    void *target_addr = (char *)dq->data + offset;

    // 拷贝元素数据到数组
    memcpy(target_addr, element_data, dq->element_size);

    // 更新队尾指针（循环移动：向后+1，超出容量则绕回）
    dq->rear = (dq->rear + 1) % (int)dq->capacity;

    // 更新元素数量
    dq->size++;
    return true;
}

/**
 * @brief 在队头添加元素（前插）
 * 核心逻辑：队头指针向前-1（通过模运算实现循环绕回）
 */
bool deque_push_front(Deque *dq, const void *element_data)
{
    // 校验参数有效性和队列状态
    if (dq == NULL || element_data == NULL || deque_is_full(dq))
    {
        return false;
    }

    // 队头指针向前移动一位（循环处理：若front=0，则绕回至capacity-1）
    // 公式：(front - 1 + capacity) % capacity 确保结果非负
    dq->front = (dq->front - 1 + (int)dq->capacity) % (int)dq->capacity;

    // 计算插入位置（移动后的front指向的位置）
    size_t offset = (size_t)dq->front * dq->element_size;
    void *target_addr = (char *)dq->data + offset;

    // 拷贝元素数据到数组
    memcpy(target_addr, element_data, dq->element_size);

    // 更新元素数量
    dq->size++;
    return true;
}

/**
 * @brief 从队头移除元素（前删）
 * 与循环队列的出队操作逻辑相同
 */
bool deque_pop_front(Deque *dq, void *output_buffer)
{
    // 校验参数有效性和队列状态
    if (dq == NULL || output_buffer == NULL || deque_is_empty(dq))
    {
        return false;
    }

    // 计算队头元素位置（front指向的位置）
    size_t offset = (size_t)dq->front * dq->element_size;
    void *source_addr = (char *)dq->data + offset;

    // 拷贝元素数据到输出缓冲区
    memcpy(output_buffer, source_addr, dq->element_size);

    // 更新队头指针（循环移动：向前+1，超出容量则绕回）
    dq->front = (dq->front + 1) % (int)dq->capacity;

    // 更新元素数量
    dq->size--;
    return true;
}

/**
 * @brief 从队尾移除元素（后删）
 * 核心逻辑：队尾指针向后-1（通过模运算实现循环绕回）
 */
bool deque_pop_back(Deque *dq, void *output_buffer)
{
    // 校验参数有效性和队列状态
    if (dq == NULL || output_buffer == NULL || deque_is_empty(dq))
    {
        return false;
    }

    // 队尾指针向后移动一位（循环处理：若rear=0，则绕回至capacity-1）
    // 公式：(rear - 1 + capacity) % capacity 确保结果非负
    dq->rear = (dq->rear - 1 + (int)dq->capacity) % (int)dq->capacity;

    // 计算队尾元素位置（移动后的rear指向的位置）
    size_t offset = (size_t)dq->rear * dq->element_size;
    void *source_addr = (char *)dq->data + offset;

    // 拷贝元素数据到输出缓冲区
    memcpy(output_buffer, source_addr, dq->element_size);

    // 更新元素数量
    dq->size--;
    return true;
}

/**
 * @brief 查看队头元素
 */
bool deque_peek_front(const Deque *dq, void *output_buffer)
{
    // 校验参数有效性和队列状态
    if (dq == NULL || output_buffer == NULL || deque_is_empty(dq))
    {
        return false;
    }

    // 计算队头元素位置并拷贝数据
    size_t offset = (size_t)dq->front * dq->element_size;
    void *source_addr = (char *)dq->data + offset;
    memcpy(output_buffer, source_addr, dq->element_size);

    return true;
}

/**
 * @brief 查看队尾元素
 */
bool deque_peek_back(const Deque *dq, void *output_buffer)
{
    // 校验参数有效性和队列状态
    if (dq == NULL || output_buffer == NULL || deque_is_empty(dq))
    {
        return false;
    }

    // 计算队尾元素位置（rear的前一位）
    int last_idx = (dq->rear - 1 + (int)dq->capacity) % (int)dq->capacity;
    size_t offset = (size_t)last_idx * dq->element_size;
    void *source_addr = (char *)dq->data + offset;

    // 拷贝数据到输出缓冲区
    memcpy(output_buffer, source_addr, dq->element_size);
    return true;
}

