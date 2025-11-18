/**
 * @file main.c
 * @brief 链式栈的测试程序
 * 包含自定义结构体存储测试和括号匹配检查测试
 */
#include "linked_stack.h"

#include <stdio.h>
#include <string.h>

/**
 * @brief 测试用自定义结构体
 * 用于验证栈对复杂数据类型的支持
 */
typedef struct
{
    int id;        // 编号
    char name[20]; // 名称
} Record;

/**
 * @brief 打印 Record 结构体
 * @param r 指向 Record 的指针（可为 NULL）
 */
void print_record(const Record *r)
{
    if (r)
    {
        printf("Record(id: %d, name: \"%s\")", r->id, r->name);
    }
    else
    {
        printf("NULL");
    }
}

/**
 * @brief 测试栈对结构体类型的支持
 */
void test_record_stack()
{
    printf("--- 测试结构体（Record）链式栈 ---\n");

    // 创建存储 Record 类型的栈（元素大小为 sizeof(Record)）
    Stack *record_stack = stack_create(sizeof(Record));
    if (!record_stack)
    {
        printf("创建栈失败！\n");
        return;
    }

    // 初始状态检查
    printf("栈创建成功。是否为空？%s\n",
           stack_is_empty(record_stack) ? "是" : "否");

    // 准备测试数据
    Record r1 = {1, "Alice"};
    Record r2 = {2, "Bob"};
    Record r3 = {3, "Charlie"};

    // 压栈操作
    printf("压入 ");
    print_record(&r1);
    printf("...\n");
    stack_push(record_stack, &r1);

    printf("压入 ");
    print_record(&r2);
    printf("...\n");
    stack_push(record_stack, &r2);

    printf("压入 ");
    print_record(&r3);
    printf("...\n");
    stack_push(record_stack, &r3);

    // 检查栈大小
    printf("当前栈大小: %zu\n", stack_get_size(record_stack));

    // 查看栈顶元素
    Record peeked;
    if (stack_peek(record_stack, &peeked))
    {
        printf("栈顶元素: ");
        print_record(&peeked);
        printf("\n");
    }

    // 弹栈并打印所有元素
    printf("\n弹出所有元素:\n");
    while (!stack_is_empty(record_stack))
    {
        Record popped;
        if (stack_pop(record_stack, &popped))
        {
            printf("弹出: ");
            print_record(&popped);
            printf(" | 剩余大小: %zu\n", stack_get_size(record_stack));
        }
    }

    // 检查栈空状态
    printf("\n栈现在是否为空？%s\n",
           stack_is_empty(record_stack) ? "是" : "否");

    // 销毁栈并检查指针
    stack_destroy(&record_stack);
    printf("栈已销毁。指针状态: %s\n\n",
           record_stack == NULL ? "NULL（安全）" : "非NULL（危险）");
}

/**
 * @brief 括号匹配检查辅助函数：判断是否为左括号
 * @param c 待检查字符
 * @return 是左括号返回 true，否则返回 false
 */
static bool is_opener(char c)
{
    return c == '(' || c == '{' || c == '[';
}

/**
 * @brief 括号匹配检查辅助函数：判断左右括号是否匹配
 * @param opener 左括号
 * @param closer 右括号
 * @return 匹配返回 true，否则返回 false
 */
static bool is_match(char opener, char closer)
{
    return (opener == '(' && closer == ')') ||
           (opener == '{' && closer == '}') || (opener == '[' && closer == ']');
}

/**
 * @brief 检查字符串中的括号是否匹配
 * @param code 待检查的字符串
 * @return 匹配返回 true，否则返回 false
 */
bool check_brackets(const char *code)
{
    // 边界检查：空字符串视为有效
    if (code == NULL)
    {
        return true;
    }

    // 创建存储 char 类型的栈（用于存储左括号）
    Stack *bracket_stack = stack_create(sizeof(char));
    if (!bracket_stack)
    {
        fprintf(stderr, "创建括号栈失败！\n");
        return false;
    }

    // 遍历字符串中的每个字符
    size_t len = strlen(code);
    for (size_t i = 0; i < len; i++)
    {
        char c = code[i];

        if (is_opener(c))
        {
            // 左括号：压入栈
            stack_push(bracket_stack, &c);
        }
        else if (c == ')' || c == '}' || c == ']')
        {
            // 右括号：检查匹配
            if (stack_is_empty(bracket_stack))
            {
                // 错误：右括号没有对应的左括号
                fprintf(stderr, "错误：位置 %zu 存在多余的右括号 '%c'\n", i, c);
                stack_destroy(&bracket_stack);
                return false;
            }

            // 弹出栈顶左括号并检查匹配
            char top_opener;
            stack_pop(bracket_stack, &top_opener);
            if (!is_match(top_opener, c))
            {
                // 错误：括号类型不匹配
                fprintf(stderr, "错误：位置 %zu 括号不匹配：'%c' 与 '%c'\n", i,
                        top_opener, c);
                stack_destroy(&bracket_stack);
                return false;
            }
        }
        // 非括号字符：忽略
    }

    // 检查是否有未匹配的左括号
    if (!stack_is_empty(bracket_stack))
    {
        char unclosed;
        stack_peek(bracket_stack, &unclosed);
        fprintf(stderr, "错误：存在未关闭的左括号 '%c'\n", unclosed);
        stack_destroy(&bracket_stack);
        return false;
    }

    // 所有检查通过
    stack_destroy(&bracket_stack);
    return true;
}

/**
 * @brief 测试括号匹配功能
 */
void test_bracket_check()
{
    printf("--- 测试括号匹配检查 ---\n");

    // 测试用例：包含有效和无效情况
    const char *test_cases[] = {
        "int main() { int x = (1 + 2); return 0; }", // 有效
        "void func(int a[]);",                       // 有效
        "([{}])",                                    // 有效
        "int arr[5] = {1, 2, 3};",                   // 有效
        "",                                          // 有效（空字符串）
        "abc",                                       // 有效（无括号）
        "([)]",                                      // 无效（交叉匹配）
        "((()",                                      // 无效（左括号未关闭）
        "())",                                       // 无效（右括号多余）
        "if (x > 0) { printf(\"hello\");",           // 无效（缺少 }）
        "int y = { ( [ ] ) };"                       // 有效（嵌套正确）
    };

    // 执行所有测试用例
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    for (int i = 0; i < num_cases; i++)
    {
        printf("检查: \"%s\"\n", test_cases[i]);
        if (check_brackets(test_cases[i]))
        {
            printf("结果: 有效\n\n");
        }
        else
        {
            printf("结果: 无效\n\n");
        }
    }
}

/**
 * @brief 主函数：执行测试
 */
int main()
{
    // 测试结构体栈（取消注释即可运行）
    // test_record_stack();

    // 测试括号匹配
    test_bracket_check();

    return 0;
}

