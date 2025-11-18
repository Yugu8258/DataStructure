/**
 * @file linked_stack.c
 * @brief 链式栈的具体实现
 * 基于单向链表实现，每个节点存储一个元素，栈顶指向链表头部
 */
#include "linked_stack.h"

#include <stdlib.h> // 提供 malloc、free
#include <string.h> // 提供 memcpy

// 链表节点结构（内部实现，用户不可见）
typedef struct Node
{
    void *data;        // 存储元素数据（动态分配，大小为 elem_size）
    struct Node *next; // 指向后一个节点（栈底方向）
} Node;

// 栈的管理结构（内部实现，用户不可见）
struct StackNode
{
    Node *top;        // 栈顶节点（最新入栈的元素）
    size_t elem_size; // 每个元素的固定大小（字节）
    size_t size;      // 当前栈中元素的数量
};

/**
 * 创建栈：初始化栈结构
 */
Stack *stack_create(size_t elem_size)
{
    // 元素大小为 0 时无效（无法存储数据）
    if (elem_size == 0)
    {
        return NULL;
    }

    // 分配栈管理结构内存
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    if (stack == NULL)
    {
        return NULL; // 内存分配失败
    }

    // 初始化栈状态
    stack->top = NULL;            // 空栈，栈顶为 NULL
    stack->elem_size = elem_size; // 记录元素大小
    stack->size = 0;              // 初始元素数量为 0

    return stack;
}

/**
 * 销毁栈：释放所有节点和栈结构
 */
void stack_destroy(Stack **stack)
{
    // 防御性检查：避免空指针解引用
    if (stack == NULL || *stack == NULL)
    {
        return;
    }

    // 清空栈中所有节点
    stack_clear(*stack);

    // 释放栈管理结构
    free(*stack);
    *stack = NULL; // 置空外部指针
}

/**
 * 压栈：在栈顶插入新元素
 */
bool stack_push(Stack *stack, const void *elem)
{
    // 检查参数有效性
    if (stack == NULL || elem == NULL)
    {
        return false;
    }

    // 1. 创建新节点
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL)
    {
        return false; // 内存不足
    }

    // 2. 为节点分配数据内存（大小为 elem_size）
    new_node->data = malloc(stack->elem_size);
    if (new_node->data == NULL)
    {
        free(new_node); // 回滚：释放已分配的节点
        return false;   // 内存不足
    }

    // 3. 拷贝元素数据到节点（保证类型无关性）
    memcpy(new_node->data, elem, stack->elem_size);

    // 4. 将新节点插入栈顶（链表头部）
    new_node->next = stack->top; // 新节点的 next 指向原栈顶
    stack->top = new_node;       // 更新栈顶为新节点

    // 5. 更新元素数量
    stack->size++;
    return true;
}

/**
 * 弹栈：移除并返回栈顶元素
 */
bool stack_pop(Stack *stack, void *elem)
{
    // 检查参数有效性和栈状态
    if (stack == NULL || elem == NULL || stack_is_empty(stack))
    {
        return false;
    }

    // 1. 暂存栈顶节点（待删除）
    Node *top_node = stack->top;

    // 2. 拷贝数据到用户缓冲区
    memcpy(elem, top_node->data, stack->elem_size);

    // 3. 更新栈顶指针
    stack->top = top_node->next;

    // 4. 释放节点资源
    free(top_node->data); // 释放元素数据
    free(top_node);       // 释放节点

    // 5. 更新元素数量
    stack->size--;
    return true;
}

/**
 * 查看栈顶元素：不修改栈结构
 */
bool stack_peek(const Stack *stack, void *elem)
{
    // 检查参数有效性和栈状态
    if (stack == NULL || elem == NULL || stack_is_empty(stack))
    {
        return false;
    }

    // 拷贝栈顶元素数据到用户缓冲区
    memcpy(elem, stack->top->data, stack->elem_size);
    return true;
}

/**
 * 检查栈是否为空
 */
bool stack_is_empty(const Stack *stack)
{
    // 空指针视为空栈
    if (stack == NULL)
    {
        return true;
    }
    // 栈顶为 NULL 或 size 为 0 均表示空栈（双重校验）
    return (stack->top == NULL) || (stack->size == 0);
}

/**
 * 获取栈中元素数量
 */
size_t stack_size(const Stack *stack)
{
    // 空指针返回 0
    if (stack == NULL)
    {
        return 0;
    }
    return stack->size;
}

/**
 * 清空栈：删除所有元素，保留栈结构
 */
void stack_clear(Stack *stack)
{
    if (stack == NULL)
    {
        return;
    }

    // 遍历链表，释放所有节点
    Node *current = stack->top;
    while (current != NULL)
    {
        Node *temp = current;
        current = current->next; // 移动到下一个节点
        free(temp->data);        // 释放元素数据
        free(temp);              // 释放节点
    }

    // 重置栈状态（保留栈结构）
    stack->top = NULL;
    stack->size = 0;
}

