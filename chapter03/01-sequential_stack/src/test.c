#include "sequentail_satck.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @file test.c
 * @brief 順序棧的功能測試程序
 * @details 測試int類型和自定義結構體類型的棧操作, 驗證所有API的正確性.
 */

// 自定義結構體: 用於測試複雜類型的棧操作
typedef struct data_point
{
    int id;       // 標識符
    double value; // 數值
} data_point;

/**
 * @brief 打印data_point結構體
 * @param dp 指向data_point的指針 (NULL時打印提示)
 */
static void _print_datapoint(const data_point *dp);

/**
 * @brief 測試整數類型的棧操作
 */
static void _test_int_stack(void);

/**
 * @brief 測試自定義結構體類型的棧操作
 */
static void _test_struct_stack(void);

int main(int argc, char *argv[])
{
    puts("\n順序棧的ADT實現\n");

    // 測試整數棧
    _test_int_stack();

    // 測試結構體棧
    _test_int_stack();

    return EXIT_SUCCESS;
}

static void _print_datapoint(const data_point *dp)
{
    if (!dp)
    {
        printf("(NULL DataPoint)");
        return;
    }

    printf("DataPoint{id: %d, value: %.2f}", dp->id, dp->value);
}

static void _test_int_stack(void)
{
    printf("=== 测试整数类型栈 ===\n");

    // 创建容量为5的int类型栈
    Stack *int_stack = stack_create(5, sizeof(int));
    if (int_stack == NULL)
    {
        printf("❌ 整数栈创建失败\n");
        return;
    }

    // 验证初始状态
    printf("初始状态 - 容量: %zu, 是否为空: %s\n",
           stack_get_capacity(int_stack),
           stack_is_empty(int_stack) ? "是" : "否");

    // 压栈测试
    int values[] = {10, 20, 30, 40, 50};
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++)
    {
        bool success = stack_push(int_stack, &values[i]);
        printf("压入 %d: %s, 当前大小: %zu\n", values[i],
               success ? "成功" : "失败", stack_get_size(int_stack));
    }

    // 测试栈满状态
    int extra = 60;
    bool push_full = stack_push(int_stack, &extra);
    printf("栈满时压入 %d: %s\n", extra,
           push_full ? "成功（错误）" : "失败（正确）");

    // 查看栈顶元素
    int top_val;
    bool peek_success = stack_peek(int_stack, &top_val);
    printf("查看栈顶元素: %s, 值: %d\n", peek_success ? "成功" : "失败",
           top_val);

    // 弹栈测试
    printf("弹栈顺序: ");
    while (!stack_is_empty(int_stack))
    {
        int popped;
        stack_pop(int_stack, &popped); // 此处已通过断言确保栈非空
        printf("%d ", popped);
    }
    printf("\n");

    // 测试栈空状态
    int dummy;
    bool pop_empty = stack_peek(int_stack, &dummy); // 用peek验证空栈
    printf("栈空时查看元素: %s\n", pop_empty ? "成功（错误）" : "失败（正确）");

    // 销毁栈
    stack_destroy(&int_stack);
    printf("销毁后指针状态: %s\n\n",
           int_stack == NULL ? "已置空（正确）" : "未置空（错误）");
}

static void _test_struct_stack(void)
{
    printf("=== 测试结构体类型栈 ===\n");

    // 创建容量为3的DataPoint类型栈
    Stack *dp_stack = stack_create(3, sizeof(data_point));
    if (dp_stack == NULL)
    {
        printf("❌ 结构体栈创建失败\n");
        return;
    }

    // 准备测试数据
    data_point p1 = {101, 99.5};
    data_point p2 = {102, 120.75};
    data_point p3 = {103, 85.0};

    // 压栈测试
    printf("压入 ");
    _print_datapoint(&p1);
    printf(": ");
    bool s1 = stack_push(dp_stack, &p1);
    printf("%s\n", s1 ? "成功" : "失败");

    printf("压入 ");
    _print_datapoint(&p2);
    printf(": ");
    bool s2 = stack_push(dp_stack, &p2);
    printf("%s\n", s2 ? "成功" : "失败");

    printf("压入 ");
    _print_datapoint(&p3);
    printf(": ");
    bool s3 = stack_push(dp_stack, &p3);
    printf("%s\n", s3 ? "成功" : "失败");

    // 验证栈状态
    printf("当前栈大小: %zu, 是否已满: %s\n", stack_get_size(dp_stack),
           stack_is_full(dp_stack) ? "是" : "否");

    // 查看栈顶元素
    data_point peeked;
    bool peek_ok = stack_peek(dp_stack, &peeked);
    printf("查看栈顶元素: ");
    if (peek_ok)
    {
        _print_datapoint(&peeked);
        printf("\n");
    }
    else
    {
        printf("失败\n");
    }

    // 弹栈测试
    printf("弹栈顺序:\n");
    while (!stack_is_empty(dp_stack))
    {
        data_point popped;
        stack_pop(dp_stack, &popped);
        printf("  弹出: ");
        _print_datapoint(&popped);
        printf("\n");
    }

    // 销毁栈
    stack_destroy(&dp_stack);
    printf("结构体栈已销毁\n");
}

