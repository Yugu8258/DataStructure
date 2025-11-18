/**
 * @file main.c
 * @brief 共享栈 ADT 的测试程序
 * 测试基本操作、边界情况及两个栈的交互（如共享空间用尽）
 */
#include "shared_stack.h"

#include <stdio.h>
#include <string.h>

// 测试 1：基本操作（int 类型）
void test_int_shared_stack(void)
{
    printf("=== 测试基本类型（int）共享栈 ===\n");

    // 创建总容量为 5 的共享栈（存储 int 类型）
    SharedStack *stack = shared_stack_create(5, sizeof(int));
    if (stack == NULL)
    {
        printf("创建共享栈失败！\n");
        return;
    }

    // 初始状态检查
    printf("栈 A 是否为空？%s（预期：是）\n",
           shared_stack_is_empty(stack, STACK_A) ? "是" : "否");
    printf("栈 B 是否为空？%s（预期：是）\n",
           shared_stack_is_empty(stack, STACK_B) ? "是" : "否");
    printf("共享栈是否已满？%s（预期：否）\n",
           shared_stack_is_full(stack) ? "是" : "否");

    // 向栈 A 压入元素
    int a1 = 10, a2 = 20;
    printf("栈 A 压入 %d...%s\n", a1,
           shared_stack_push(stack, STACK_A, &a1) ? "成功" : "失败");
    printf("栈 A 压入 %d...%s\n", a2,
           shared_stack_push(stack, STACK_A, &a2) ? "成功" : "失败");
    printf("栈 A 大小：%zu（预期：2）\n", shared_stack_size(stack, STACK_A));

    // 向栈 B 压入元素
    int b1 = 30, b2 = 40, b3 = 50;
    printf("栈 B 压入 %d...%s\n", b1,
           shared_stack_push(stack, STACK_B, &b1) ? "成功" : "失败");
    printf("栈 B 压入 %d...%s\n", b2,
           shared_stack_push(stack, STACK_B, &b2) ? "成功" : "失败");
    printf("栈 B 压入 %d...%s\n", b3,
           shared_stack_push(stack, STACK_B, &b3) ? "成功" : "失败");
    printf("栈 B 大小：%zu（预期：3）\n", shared_stack_size(stack, STACK_B));

    // 检查共享栈是否已满（总容量 5，2+3=5）
    printf("共享栈是否已满？%s（预期：是）\n",
           shared_stack_is_full(stack) ? "是" : "否");

    // 查看栈顶元素
    int peek_a, peek_b;
    shared_stack_peek(stack, STACK_A, &peek_a);
    shared_stack_peek(stack, STACK_B, &peek_b);
    printf("栈 A 顶：%d（预期：20），栈 B 顶：%d（预期：50）\n", peek_a,
           peek_b);

    // 栈 A 弹栈
    int pop_a;
    shared_stack_pop(stack, STACK_A, &pop_a);
    printf("栈 A 弹出：%d（预期：20），剩余大小：%zu（预期：1）\n", pop_a,
           shared_stack_size(stack, STACK_A));

    // 此时共享栈未满，可继续向栈 B 压入
    int b4 = 60;
    printf("栈 B 压入 %d...%s（预期：成功）\n", b4,
           shared_stack_push(stack, STACK_B, &b4) ? "成功" : "失败");
    printf("栈 B 大小：%zu（预期：4）\n", shared_stack_size(stack, STACK_B));

    // 清空栈 A
    shared_stack_clear(stack, STACK_A);
    printf("清空后栈 A 是否为空？%s（预期：是）\n",
           shared_stack_is_empty(stack, STACK_A) ? "是" : "否");

    // 销毁共享栈
    shared_stack_destroy(&stack);
    printf("销毁后栈指针是否为 NULL？%s（预期：是）\n\n",
           stack == NULL ? "是" : "否");
}

// 测试 2：自定义类型（结构体）
typedef struct
{
    char name[10];
    float score;
} Student;

void print_student(const Student *s)
{
    if (s)
    {
        printf("Student{name: %s, score: %.1f}", s->name, s->score);
    }
    else
    {
        printf("NULL");
    }
}

void test_struct_shared_stack(void)
{
    printf("=== 测试自定义类型（Student）共享栈 ===\n");

    // 创建总容量为 3 的共享栈（存储 Student 类型）
    SharedStack *stack = shared_stack_create(3, sizeof(Student));
    if (stack == NULL)
    {
        printf("创建共享栈失败！\n");
        return;
    }

    // 栈 A 压入数据
    Student s1 = {"Alice", 90.5};
    Student s2 = {"Bob", 85.0};
    printf("栈 A 压入 ");
    print_student(&s1);
    printf("...%s\n", shared_stack_push(stack, STACK_A, &s1) ? "成功" : "失败");
    printf("栈 A 压入 ");
    print_student(&s2);
    printf("...%s\n", shared_stack_push(stack, STACK_A, &s2) ? "成功" : "失败");

    // 栈 B 压入数据（总容量 3，栈 A 已用 2，栈 B 可压入 1 个）
    Student s3 = {"Charlie", 92.0};
    printf("栈 B 压入 ");
    print_student(&s3);
    printf("...%s\n", shared_stack_push(stack, STACK_B, &s3) ? "成功" : "失败");

    // 尝试向栈 B 压入第 2 个元素（预期失败，总容量已满）
    Student s4 = {"Dave", 78.5};
    printf("栈 B 压入 ");
    print_student(&s4);
    printf("...%s（预期：失败）\n",
           shared_stack_push(stack, STACK_B, &s4) ? "成功" : "失败");

    // 栈 B 弹栈并查看
    Student pop_b;
    shared_stack_pop(stack, STACK_B, &pop_b);
    printf("栈 B 弹出：");
    print_student(&pop_b);
    printf("（预期：Student{name: Charlie, score: 92.0}）\n");

    // 销毁共享栈
    shared_stack_destroy(&stack);
}

int main(void)
{
    test_int_shared_stack();    // 测试基本类型
    test_struct_shared_stack(); // 测试自定义结构体
    return 0;
}

