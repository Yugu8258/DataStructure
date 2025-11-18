/**
 * @file generic_linked_queue.c
 * @brief 通用链式队列的具体实现
 * 内部通过单向链表实现，队头（front）用于出队，队尾（rear）用于入队
 */
#include "generic_linked_queue.h"

#include <stdlib.h> // 提供 malloc、free
#include <string.h> // 提供 memcpy

// 链表节点结构（内部实现，用户不可见）
typedef struct Node
{
    void *data;        // 存储元素数据（动态分配，大小为 element_size）
    struct Node *next; // 指向后一个节点（队尾方向）
} Node;

// 队列管理结构（内部实现，用户不可见）
struct LinkedQueue
{
    Node *front;         // 队头指针（指向首个元素）
    Node *rear;          // 队尾指针（指向最后一个元素）
    size_t element_size; // 每个元素的大小（字节）
    size_t size;         // 当前元素数量
};

/**
 * @brief 创建队列
 */
Queue *queue_create(size_t element_size)
{
    // 校验参数：元素大小必须大于0
    if (element_size == 0)
    {
        return NULL;
    }

    // 分配队列管理结构
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    if (queue == NULL)
    {
        return NULL; // 内存不足
    }

    // 初始化队列状态
    queue->front = NULL;                // 空队列，队头为 NULL
    queue->rear = NULL;                 // 空队列，队尾为 NULL
    queue->element_size = element_size; // 记录元素大小
    queue->size = 0;                    // 初始元素数量为 0

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

    Queue *queue = *p_queue;
    Node *current = queue->front;

    // 遍历链表，释放所有节点
    while (current != NULL)
    {
        Node *temp = current;
        current = current->next; // 移动到下一个节点
        free(temp->data);        // 释放节点存储的元素数据
        free(temp);              // 释放节点本身
    }

    // 释放队列管理结构并置空外部指针
    free(queue);
    *p_queue = NULL;
}

/**
 * @brief 入队操作
 */
bool queue_enqueue(Queue *queue, const void *element_data)
{
    // 校验参数有效性
    if (queue == NULL || element_data == NULL)
    {
        return false;
    }

    // 1. 创建新节点
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL)
    {
        return false; // 内存不足
    }

    // 2. 为节点数据区分配内存
    new_node->data = malloc(queue->element_size);
    if (new_node->data == NULL)
    {
        free(new_node); // 回滚：释放已分配的节点
        return false;   // 内存不足
    }

    // 3. 拷贝元素数据到节点（支持任意类型）
    memcpy(new_node->data, element_data, queue->element_size);
    new_node->next = NULL; // 新节点位于队尾，next 为 NULL

    // 4. 链接新节点到队列
    if (queue_is_empty(queue))
    {
        // 特殊情况：队列为空时，队头和队尾都指向新节点
        queue->front = new_node;
        queue->rear = new_node;
    }
    else
    {
        // 常规情况：新节点链接到当前队尾的后面，更新队尾指针
        queue->rear->next = new_node;
        queue->rear = new_node;
    }

    // 5. 更新元素数量
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

    // 1. 暂存队头节点（待删除）
    Node *front_node = queue->front;

    // 2. 拷贝队头元素数据到输出缓冲区
    memcpy(output_buffer, front_node->data, queue->element_size);

    // 3. 更新队头指针
    queue->front = front_node->next;

    // 4. 特殊情况：如果队头变为 NULL，说明队列已空，需同步更新队尾
    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    // 5. 释放原队头节点的内存
    free(front_node->data); // 释放数据区
    free(front_node);       // 释放节点

    // 6. 更新元素数量
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

    // 仅拷贝队头元素数据，不修改队列结构
    memcpy(output_buffer, queue->front->data, queue->element_size);
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
    return queue->size == 0; // 也可通过 queue->front == NULL 判断
}

/**
 * @brief 获取队列元素数量
 */
size_t queue_get_size(const Queue *queue)
{
    if (queue == NULL)
    {
        return 0;
    }
    return queue->size;
}

