/**
 * @file main.c
 * @brief 链式栈 ADT 的测试程序
 * 测试基本操作、边界情况及自定义数据类型的支持
 */
#include "linked_stack.h"

#include <stdio.h>
#include <string.h>

// 测试 1：基本数据类型（int）
void test_int_stack(void)
{
    printf("=== 测试基本类型（int） ===\n");

    // 创建存储 int 类型的栈（元素大小为 sizeof(int)）
    Stack *int_stack = stack_create(sizeof(int));
    if (int_stack == NULL)
    {
        printf("创建栈失败！\n");
        return;
    }

    // 测试初始状态
    printf("初始栈是否为空？%s（预期：是）\n",
           stack_is_empty(int_stack) ? "是" : "否");
    printf("初始栈大小：%zu（预期：0）\n", stack_size(int_stack));

    // 压栈操作
    int a = 10, b = 20, c = 30;
    printf("压栈 %d...%s\n", a, stack_push(int_stack, &a) ? "成功" : "失败");
    printf("压栈 %d...%s\n", b, stack_push(int_stack, &b) ? "成功" : "失败");
    printf("压栈 %d...%s\n", c, stack_push(int_stack, &c) ? "成功" : "失败");

    // 测试压栈后状态
    printf("压栈后大小：%zu（预期：3）\n", stack_size(int_stack));
    int peek_val;
    stack_peek(int_stack, &peek_val);
    printf("栈顶元素：%d（预期：30）\n", peek_val);

    // 弹栈操作
    int pop_val;
    stack_pop(int_stack, &pop_val);
    printf("弹出元素：%d（预期：30），剩余大小：%zu（预期：2）\n", pop_val,
           stack_size(int_stack));
    stack_pop(int_stack, &pop_val);
    printf("弹出元素：%d（预期：20），剩余大小：%zu（预期：1）\n", pop_val,
           stack_size(int_stack));

    // 清空栈
    stack_clear(int_stack);
    printf("清空后栈是否为空？%s（预期：是）\n",
           stack_is_empty(int_stack) ? "是" : "否");

    // 销毁栈
    stack_destroy(&int_stack);
    printf("销毁后栈指针是否为 NULL？%s（预期：是）\n\n",
           int_stack == NULL ? "是" : "否");
}

// 测试 2：自定义数据类型（结构体）
typedef struct
{
    char name[20];
    int age;
} Person;

// 打印 Person 信息
void print_person(const Person *p)
{
    if (p)
    {
        printf("Person{name: %s, age: %d}", p->name, p->age);
    }
    else
    {
        printf("NULL");
    }
}

void test_struct_stack(void)
{
    printf("=== 测试自定义类型（Person） ===\n");

    // 创建存储 Person 类型的栈
    Stack *person_stack = stack_create(sizeof(Person));
    if (person_stack == NULL)
    {
        printf("创建栈失败！\n");
        return;
    }

    // 准备测试数据
    Person p1 = {"Alice", 25};
    Person p2 = {"Bob", 30};

    // 压栈
    printf("压栈 ");
    print_person(&p1);
    printf("...%s\n", stack_push(person_stack, &p1) ? "成功" : "失败");
    printf("压栈 ");
    print_person(&p2);
    printf("...%s\n", stack_push(person_stack, &p2) ? "成功" : "失败");

    // 查看栈顶
    Person top_p;
    stack_peek(person_stack, &top_p);
    printf("栈顶元素：");
    print_person(&top_p);
    printf("（预期：Person{name: Bob, age: 30}）\n");

    // 弹栈
    Person pop_p;
    stack_pop(person_stack, &pop_p);
    printf("弹出元素：");
    print_person(&pop_p);
    printf("，剩余大小：%zu（预期：1）\n", stack_size(person_stack));

    // 销毁栈
    stack_destroy(&person_stack);
    printf("销毁后栈指针是否为 NULL？%s（预期：是）\n\n",
           person_stack == NULL ? "是" : "否");
}

// 测试 3：边界情况（空栈弹栈、NULL 参数等）
void test_edge_cases(void)
{
    printf("=== 测试边界情况 ===\n");

    // 测试 NULL 栈的行为
    Stack *null_stack = NULL;
    printf("NULL 栈是否为空？%s（预期：是）\n",
           stack_is_empty(null_stack) ? "是" : "否");
    printf("NULL 栈的大小：%zu（预期：0）\n", stack_size(null_stack));

    // 创建正常栈
    Stack *stack = stack_create(sizeof(char));
    // 空栈弹栈
    char c;
    printf("空栈弹栈是否失败？%s（预期：是）\n",
           stack_pop(stack, &c) ? "否" : "是");
    // 空栈查看栈顶
    printf("空栈查看栈顶是否失败？%s（预期：是）\n",
           stack_peek(stack, &c) ? "否" : "是");
    // 向 NULL 栈压栈
    char ch = 'a';
    printf("向 NULL 栈压栈是否失败？%s（预期：是）\n",
           stack_push(null_stack, &ch) ? "否" : "是");

    // 销毁栈
    stack_destroy(&stack);
}

int main(void)
{
    test_int_stack();    // 测试基本类型
    test_struct_stack(); // 测试自定义结构体
    test_edge_cases();   // 测试边界情况
    return 0;
}

