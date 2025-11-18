/**
 * @file main.c
 * @brief 循环队列的测试程序
 * 验证队列的基本操作、循环特性及边界情况（移除对内部成员的直接访问）
 */
#include "generic_circular_queue.h"

#include <stdio.h>

/**
 * @brief 打印队列的公开状态（不访问内部成员）
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

    // 仅打印通过公开接口获取的状态
    printf("容量: %zu, 当前大小: %zu\n", queue_get_capacity(q),
           queue_get_size(q));
    printf("是否为空: %s, 是否已满: %s\n\n", queue_is_empty(q) ? "是" : "否",
           queue_is_full(q) ? "是" : "否");
}

/**
 * @brief 测试基本数据类型（int）的队列操作
 */
void test_int_queue(void)
{
    printf("=== 测试 int 类型循环队列 ===\n");

    // 1. 创建容量为5的int队列
    Queue *q = queue_create(5, sizeof(int));
    print_queue_status(q, "1. 初始化队列");

    // 2. 入队3个元素
    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++)
    {
        printf("入队元素: %d\n", values[i]);
        queue_enqueue(q, &values[i]);
    }
    print_queue_status(q, "2. 入队 10, 20, 30 后");

    // 3. 出队2个元素
    int dequeued;
    for (int i = 0; i < 2; i++)
    {
        if (queue_dequeue(q, &dequeued))
        {
            printf("出队元素: %d\n", dequeued);
        }
    }
    print_queue_status(q, "3. 出队2个元素后");

    // 4. 继续入队，验证循环特性（通过逻辑说明，不访问内部指针）
    int more_values[] = {40, 50, 60};
    printf("验证循环特性：继续入队 40, 50, 60（此时队列会绕回数组头部）\n");
    for (int i = 0; i < 3; i++)
    {
        printf("入队元素: %d\n", more_values[i]);
        queue_enqueue(q, &more_values[i]);
    }
    print_queue_status(q, "4. 入队后（已循环）");

    // 5. 查看队头元素
    int front_val;
    if (queue_peek(q, &front_val))
    {
        printf("当前队头元素: %d（预期：30）\n\n", front_val);
    }

    // 6. 测试队列满时入队（预期失败）
    int overflow = 999;
    printf("尝试向满队列入队 %d...", overflow);
    if (!queue_enqueue(q, &overflow))
    {
        printf("入队失败（符合预期）\n\n");
    }

    // 7. 清空队列
    printf("出队所有元素：\n");
    while (!queue_is_empty(q))
    {
        queue_dequeue(q, &dequeued);
        printf("出队元素: %d\n", dequeued);
    }
    print_queue_status(q, "7. 清空队列后");

    // 8. 销毁队列
    queue_destroy(&q);
    printf("销毁队列后，指针状态: %s\n\n",
           q == NULL ? "NULL（安全）" : "非NULL（危险）");
}

/**
 * @brief 测试自定义结构体类型的队列操作
 */
typedef struct
{
    char name[10];
    int id;
} User;

void print_user(const User *u)
{
    if (u)
    {
        printf("User{name: %s, id: %d}", u->name, u->id);
    }
}

void test_struct_queue(void)
{
    printf("=== 测试自定义结构体（User）队列 ===\n");

    // 创建容量为2的User队列
    Queue *q = queue_create(2, sizeof(User));
    if (q == NULL)
    {
        printf("创建队列失败！\n");
        return;
    }

    // 入队两个User
    User u1 = {"Alice", 1001};
    User u2 = {"Bob", 1002};
    printf("入队 ");
    print_user(&u1);
    printf("...\n");
    queue_enqueue(q, &u1);
    printf("入队 ");
    print_user(&u2);
    printf("...\n");
    queue_enqueue(q, &u2);
    print_queue_status(q, "入队两个User后（队列满）");

    // 出队一个User
    User dequeued_user;
    queue_dequeue(q, &dequeued_user);
    printf("出队元素：");
    print_user(&dequeued_user);
    printf("\n");

    // 再入队一个User（验证循环）
    User u3 = {"Charlie", 1003};
    printf("入队 ");
    print_user(&u3);
    printf("...\n");
    queue_enqueue(q, &u3);
    print_queue_status(q, "入队第三个User后（循环）");

    // 销毁队列
    queue_destroy(&q);
}

int main(void)
{
    test_int_queue();    // 测试基本类型
    test_struct_queue(); // 测试自定义结构体
    return 0;
}

