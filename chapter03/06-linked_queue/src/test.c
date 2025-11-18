/**
 * @file main.c
 * @brief 链式队列的测试程序
 * 验证队列的创建、入队、出队、查看等操作及边界情况
 */
#include "generic_linked_queue.h"

#include <stdio.h>

/**
 * @brief 测试用自定义结构体
 */
typedef struct
{
    int id;        // 编号
    char name[16]; // 名称
} Person;

/**
 * @brief 打印 Person 结构体信息
 * @param p 指向 Person 的指针（可为 NULL）
 */
void print_person(const Person *p)
{
    if (p != NULL)
    {
        printf("Person(id: %d, name: \"%s\")", p->id, p->name);
    }
    else
    {
        printf("NULL");
    }
}

/**
 * @brief 打印队列当前状态（通过公开接口）
 * @param q 队列指针
 * @param title 测试步骤标题
 */
void print_queue_status(const Queue *q, const char *title)
{
    printf("--- %s ---\n", title);
    if (q == NULL)
    {
        printf("队列为 NULL\n\n");
        return;
    }

    // 打印队列基本状态
    printf("元素数量: %zu, 是否为空: %s\n", queue_get_size(q),
           queue_is_empty(q) ? "是" : "否");

    // 打印队头元素（若队列非空）
    Person peeked;
    if (queue_peek(q, &peeked))
    {
        printf("队头元素: ");
        print_person(&peeked);
        printf("\n");
    }
    else
    {
        printf("队头元素: 无（队列为空）\n");
    }
    printf("\n");
}

int main(void)
{
    // 1. 创建队列（存储 Person 类型）
    Queue *q = queue_create(sizeof(Person));
    print_queue_status(q, "1. 初始化队列");

    // 2. 入队第一个元素（测试空队列入队）
    Person p1 = {101, "Alice"};
    printf("入队元素: ");
    print_person(&p1);
    printf("\n");
    queue_enqueue(q, &p1);
    print_queue_status(q, "2. 入队第一个元素后");

    // 3. 入队多个元素（测试常规入队）
    Person p2 = {102, "Bob"};
    Person p3 = {103, "Charlie"};
    printf("入队元素: ");
    print_person(&p2);
    printf("\n");
    queue_enqueue(q, &p2);
    printf("入队元素: ");
    print_person(&p3);
    printf("\n");
    queue_enqueue(q, &p3);
    print_queue_status(q, "3. 入队多个元素后");

    // 4. 出队一个元素（测试常规出队）
    Person dequeued;
    if (queue_dequeue(q, &dequeued))
    {
        printf("出队元素: ");
        print_person(&dequeued);
        printf("\n");
    }
    print_queue_status(q, "4. 出队一个元素后");

    // 5. 全部出队（测试队列清空）
    printf("出队剩余所有元素:\n");
    while (!queue_is_empty(q))
    {
        queue_dequeue(q, &dequeued);
        printf("出队元素: ");
        print_person(&dequeued);
        printf("\n");
    }
    print_queue_status(q, "5. 全部出队后");

    // 6. 测试空队列操作（预期失败）
    printf("尝试对空队列出队...\n");
    if (!queue_dequeue(q, &dequeued))
    {
        printf("操作失败（符合预期）\n\n");
    }

    // 7. 销毁队列
    queue_destroy(&q);
    printf("销毁队列后，指针状态: %s\n",
           q == NULL ? "NULL（安全）" : "非NULL（危险）");

    return 0;
}

