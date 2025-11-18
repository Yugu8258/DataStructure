/**
 * @file shared_stack.c
 * @brief 共享栈的具体实现
 * 内部用一个数组存储两个栈，栈 A 从左向右生长，栈 B 从右向左生长
 */
#include "shared_stack.h"

#include <stdlib.h> // 提供 malloc、free
#include <string.h> // 提供 memcpy、memset

// 共享栈的内部结构
struct SharedStack
{
    void *data;       // 存储元素的数组（动态分配）
    size_t capacity;  // 数组总容量（最大元素数）
    size_t elem_size; // 每个元素的大小（字节）
    int top_a;        // 栈 A 的栈顶索引（初始为 -1）
    int top_b;        // 栈 B 的栈顶索引（初始为 capacity）
};

/**
 * 创建共享栈：初始化数组和栈顶指针
 */
SharedStack *shared_stack_create(size_t capacity, size_t elem_size)
{
    // 容量或元素大小为 0 时无效
    if (capacity == 0 || elem_size == 0)
    {
        return NULL;
    }

    // 分配共享栈管理结构
    SharedStack *stack = (SharedStack *)malloc(sizeof(SharedStack));
    if (stack == NULL)
    {
        return NULL;
    }

    // 分配存储元素的数组（总大小 = 容量 × 每个元素大小）
    stack->data = malloc(capacity * elem_size);
    if (stack->data == NULL)
    {
        free(stack); // 回滚：释放管理结构
        return NULL;
    }

    // 初始化栈状态
    stack->capacity = capacity;
    stack->elem_size = elem_size;
    stack->top_a = -1;            // 栈 A 初始为空（栈顶在数组左侧外）
    stack->top_b = (int)capacity; // 栈 B 初始为空（栈顶在数组右侧外）

    return stack;
}

/**
 * 销毁共享栈：释放数组和管理结构
 */
void shared_stack_destroy(SharedStack **stack)
{
    if (stack == NULL || *stack == NULL)
    {
        return;
    }

    // 释放元素数组和管理结构
    free((*stack)->data);
    free(*stack);
    *stack = NULL; // 置空外部指针，防止悬垂指针
}

/**
 * 压栈操作：向指定栈添加元素
 */
bool shared_stack_push(SharedStack *stack, StackID id, const void *elem)
{
    // 检查参数有效性和栈满状态
    if (stack == NULL || elem == NULL || shared_stack_is_full(stack))
    {
        return false;
    }

    // 计算元素在数组中的存储位置（字节偏移量）
    size_t offset;

    if (id == STACK_A)
    {
        // 栈 A 压栈：栈顶右移（索引 +1）
        stack->top_a++;
        offset = (size_t)stack->top_a * stack->elem_size;
    }
    else
    { // id == STACK_B
        // 栈 B 压栈：栈顶左移（索引 -1）
        stack->top_b--;
        offset = (size_t)stack->top_b * stack->elem_size;
    }

    // 拷贝元素数据到数组（类型无关）
    memcpy((char *)stack->data + offset, elem, stack->elem_size);
    return true;
}

/**
 * 弹栈操作：从指定栈移除元素
 */
bool shared_stack_pop(SharedStack *stack, StackID id, void *elem)
{
    // 检查参数有效性和栈空状态
    if (stack == NULL || elem == NULL || shared_stack_is_empty(stack, id))
    {
        return false;
    }

    // 计算元素在数组中的存储位置（字节偏移量）
    size_t offset;

    if (id == STACK_A)
    {
        // 栈 A 弹栈：先获取当前栈顶元素，再左移栈顶（索引 -1）
        offset = (size_t)stack->top_a * stack->elem_size;
        stack->top_a--;
    }
    else
    { // id == STACK_B
        // 栈 B 弹栈：先获取当前栈顶元素，再右移栈顶（索引 +1）
        offset = (size_t)stack->top_b * stack->elem_size;
        stack->top_b++;
    }

    // 拷贝元素数据到用户缓冲区
    memcpy(elem, (char *)stack->data + offset, stack->elem_size);
    return true;
}

/**
 * 查看栈顶元素：不修改栈结构
 */
bool shared_stack_peek(const SharedStack *stack, StackID id, void *elem)
{
    // 检查参数有效性和栈空状态
    if (stack == NULL || elem == NULL || shared_stack_is_empty(stack, id))
    {
        return false;
    }

    // 计算元素在数组中的存储位置（字节偏移量）
    size_t offset;
    if (id == STACK_A)
    {
        offset = (size_t)stack->top_a * stack->elem_size;
    }
    else
    {
        offset = (size_t)stack->top_b * stack->elem_size;
    }

    // 拷贝栈顶元素数据
    memcpy(elem, (char *)stack->data + offset, stack->elem_size);
    return true;
}

/**
 * 检查指定栈是否为空
 */
bool shared_stack_is_empty(const SharedStack *stack, StackID id)
{
    if (stack == NULL)
    {
        return true; // 空指针视为栈空
    }
    if (id == STACK_A)
    {
        return stack->top_a == -1; // 栈 A 空：top_a 回到初始位置
    }
    else
    {
        return stack->top_b ==
               (int)stack->capacity; // 栈 B 空：top_b 回到初始位置
    }
}

/**
 * 检查共享栈是否已满（两个栈顶相遇时满）
 */
bool shared_stack_is_full(const SharedStack *stack)
{
    if (stack == NULL)
    {
        return true; // 空指针视为已满（防止错误操作）
    }
    // 栈 A 顶 +1 == 栈 B 顶时，数组空间用尽
    return (stack->top_a + 1) == stack->top_b;
}

/**
 * 获取指定栈的元素数量
 */
size_t shared_stack_size(const SharedStack *stack, StackID id)
{
    if (stack == NULL)
    {
        return 0;
    }
    if (id == STACK_A)
    {
        // 栈 A 大小：top_a + 1（初始 -1 → 0 个元素）
        return (size_t)(stack->top_a + 1);
    }
    else
    {
        // 栈 B 大小：capacity - top_b（初始 capacity → 0 个元素）
        return stack->capacity - (size_t)stack->top_b;
    }
}

/**
 * 清空指定栈：重置栈顶指针（不清除数组数据，仅逻辑清空）
 */
void shared_stack_clear(SharedStack *stack, StackID id)
{
    if (stack == NULL)
    {
        return;
    }
    if (id == STACK_A)
    {
        stack->top_a = -1; // 栈 A 重置为初始状态
    }
    else
    {
        stack->top_b = (int)stack->capacity; // 栈 B 重置为初始状态
    }
}

