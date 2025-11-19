#include "priority_queue.h"

#include <stdio.h>
#include <string.h>

// --------------------------
// 内部辅助函数（仅实现文件可见）--------------------------

/**
 * @brief 交换两个PQElement类型的值
 */
static void swap(PQElement *a, PQElement *b)
{
    PQElement temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief 上浮操作：将指定索引的元素向上调整，维持最大堆特性
 * @param pq 队列指针（不可为NULL）
 * @param index 待上浮元素的索引
 */
static void heapify_up(PriorityQueue *pq, size_t index)
{
    if (!pq || index == 0 || index >= pq->size)
        return;

    while (index > 0)
    {
        size_t parent_idx = (index - 1) / 2; // 父节点索引（整数除法向下取整）

        // 最大堆特性：父节点值 >= 子节点值，若子节点更大则交换
        if (pq->data[index] > pq->data[parent_idx])
        {
            swap(&pq->data[index], &pq->data[parent_idx]);
            index = parent_idx; // 继续向上调整
        }
        else
        {
            break; // 已满足最大堆特性，退出
        }
    }
}

/**
 * @brief 下沉操作：将指定索引的元素向下调整，维持最大堆特性
 * @param pq 队列指针（不可为NULL）
 * @param index 待下沉元素的索引
 */
static void heapify_down(PriorityQueue *pq, size_t index)
{
    if (!pq || index >= pq->size)
        return;

    while (true)
    {
        size_t left_child = 2 * index + 1;  // 左子节点索引
        size_t right_child = 2 * index + 2; // 右子节点索引
        size_t max_idx = index;             // 初始假设当前节点为最大值

        // 找出当前节点、左子节点、右子节点中的最大值索引
        if (left_child < pq->size && pq->data[left_child] > pq->data[max_idx])
        {
            max_idx = left_child;
        }
        if (right_child < pq->size && pq->data[right_child] > pq->data[max_idx])
        {
            max_idx = right_child;
        }

        // 若最大值不是当前节点，交换并继续下沉
        if (max_idx != index)
        {
            swap(&pq->data[index], &pq->data[max_idx]);
            index = max_idx; // 继续向下调整
        }
        else
        {
            break; // 已满足最大堆特性，退出
        }
    }
}

/**
 * @brief 动态扩容：队列满时容量翻倍
 * @param pq 队列指针（不可为NULL）
 * @return 成功返回0，失败返回-1（内存分配失败，原有数据不变）
 */
static int pq_resize(PriorityQueue *pq)
{
    if (!pq)
        return -1;

    size_t new_capacity = pq->capacity * 2;
    // 避免容量溢出（极端情况下翻倍后超出size_t范围）
    if (new_capacity < pq->capacity)
    {
        fprintf(stderr, "Error: Priority queue capacity overflow\n");
        return -1;
    }

    // 重新分配内存（保留原有数据）
    PQElement *new_data =
        (PQElement *)realloc(pq->data, new_capacity * sizeof(PQElement));
    if (!new_data)
    {
        fprintf(stderr,
                "Error: Failed to resize priority queue (realloc failed)\n");
        return -1;
    }

    pq->data = new_data;
    pq->capacity = new_capacity;
    printf("[DEBUG] PQ resized: old capacity=%zu -> new capacity=%zu\n",
           pq->capacity / 2, pq->capacity);
    return 0;
}

// -------------------------- 公共API实现 --------------------------

PriorityQueue *pq_create(size_t initial_capacity)
{
    // 初始容量为0时，使用默认值8
    if (initial_capacity == 0)
    {
        initial_capacity = 8;
    }

    // 分配队列结构体内存
    PriorityQueue *pq = (PriorityQueue *)malloc(sizeof(PriorityQueue));
    if (!pq)
    {
        fprintf(stderr, "Error: Failed to allocate priority queue structure\n");
        return NULL;
    }

    // 分配元素存储数组内存
    pq->data = (PQElement *)malloc(initial_capacity * sizeof(PQElement));
    if (!pq->data)
    {
        fprintf(stderr,
                "Error: Failed to allocate priority queue data array\n");
        free(pq); // 回滚：释放结构体
        return NULL;
    }

    // 初始化队列状态
    pq->size = 0;
    pq->capacity = initial_capacity;
    printf("[DEBUG] PQ created: capacity=%zu\n", initial_capacity);
    return pq;
}

void pq_destroy(PriorityQueue **pq)
{
    // 双重指针确保外部指针置NULL，避免野指针
    if (pq && *pq)
    {
        free((*pq)->data); // 释放元素数组
        (*pq)->data = NULL;
        free(*pq);  // 释放队列结构体
        *pq = NULL; // 外部指针置NULL
        printf("[DEBUG] PQ destroyed successfully\n");
    }
}

int pq_push(PriorityQueue *pq, PQElement elem)
{
    // 参数校验
    if (!pq)
    {
        fprintf(stderr, "Error: Priority queue pointer is NULL\n");
        return -1;
    }

    // 队列满时扩容
    if (pq->size == pq->capacity)
    {
        if (pq_resize(pq) != 0)
        {
            return -1; // 扩容失败
        }
    }

    // 1. 新元素插入数组末尾（维持完全二叉树结构）
    pq->data[pq->size] = elem;
    // 2. 元素数量+1
    pq->size++;
    // 3. 上浮调整，维持最大堆特性（确保堆顶为最高优先级）
    heapify_up(pq, pq->size - 1);

    printf("[DEBUG] PQ pushed: %d (size=%zu)\n", elem, pq->size);
    return 0;
}

int pq_pop(PriorityQueue *pq, PQElement *p_elem)
{
    // 参数校验
    if (!pq || !p_elem)
    {
        fprintf(stderr, "Error: Invalid parameters (NULL pointer)\n");
        return -1;
    }
    if (pq_is_empty(pq))
    {
        fprintf(stderr, "Error: Cannot pop from empty priority queue\n");
        return -1;
    }

    // 1. 堆顶元素为最高优先级，存入输出参数
    *p_elem = pq->data[0];
    // 2. 最后一个元素移到堆顶（维持完全二叉树结构）
    pq->data[0] = pq->data[pq->size - 1];
    // 3. 元素数量-1
    pq->size--;
    // 4. 下沉调整，维持最大堆特性
    if (pq->size > 0)
    {
        heapify_down(pq, 0);
    }

    printf("[DEBUG] PQ popped: %d (size=%zu)\n", *p_elem, pq->size);
    return 0;
}

int pq_top(const PriorityQueue *pq, PQElement *p_elem)
{
    // 参数校验
    if (!pq || !p_elem)
    {
        fprintf(stderr, "Error: Invalid parameters (NULL pointer)\n");
        return -1;
    }
    if (pq_is_empty(pq))
    {
        fprintf(stderr, "Error: Cannot get top from empty priority queue\n");
        return -1;
    }

    // 直接返回堆顶元素（不修改队列）
    *p_elem = pq->data[0];
    printf("[DEBUG] PQ top: %d\n", *p_elem);
    return 0;
}

void pq_clear(PriorityQueue *pq)
{
    if (!pq)
        return;
    pq->size = 0; // 仅重置元素数量，不释放内存
    printf("[DEBUG] PQ cleared (size=0)\n");
}

bool pq_is_empty(const PriorityQueue *pq)
{
    return (pq && pq->size == 0);
}

size_t pq_size(const PriorityQueue *pq)
{
    return (pq ? pq->size : 0);
}

void pq_print_debug(const PriorityQueue *pq)
{
    printf("\n[DEBUG] Priority Queue Info:\n");
    if (!pq)
    {
        printf("  Queue: NULL\n");
        return;
    }

    printf("  Capacity: %zu\n", pq->capacity);
    printf("  Size: %zu\n", pq->size);
    printf("  Is Empty: %s\n", pq_is_empty(pq) ? "Yes" : "No");

    if (!pq_is_empty(pq))
    {
        printf("  Elements (order: [top, ..., bottom]): [");
        for (size_t i = 0; i < pq->size; i++)
        {
            printf("%d", pq->data[i]);
            if (i != pq->size - 1)
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

