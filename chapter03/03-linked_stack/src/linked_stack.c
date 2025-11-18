/**
 * @file linked_stack.c
 * @brief 通用链式栈的实现
 * 基于链表实现，支持任意数据类型，通过内存拷贝实现类型无关性
 */
#include "linked_stack.h"
#include <stdlib.h>  // 提供 malloc/free
#include <string.h>  // 提供 memcpy

// 链表节点结构（内部实现，用户不可见）
typedef struct Node {
    void* data;         // 存储元素数据（动态分配，大小为 element_size）
    struct Node* next;  // 指向栈的下一个节点（栈顶方向）
} Node;

// 栈的管理结构（内部实现，用户不可见）
struct LinkedStack {
    Node* top;              // 栈顶节点（最新元素）
    size_t element_size;    // 每个元素的固定大小（字节）
    size_t size;            // 当前栈中元素数量
};

/**
 * @brief 创建栈的实现
 */
Stack* stack_create(size_t element_size) {
    // 元素大小为 0 时无效（无法分配内存）
    if (element_size == 0) {
        return NULL;
    }

    // 分配栈管理结构内存
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if (stack == NULL) {
        return NULL;  // 内存不足
    }

    // 初始化栈状态
    stack->top = NULL;          // 空栈，栈顶为 NULL
    stack->element_size = element_size;  // 记录元素大小
    stack->size = 0;            // 初始元素数量为 0

    return stack;
}

/**
 * @brief 销毁栈的实现
 */
void stack_destroy(Stack** p_stack) {
    // 避免空指针解引用（防御性检查）
    if (p_stack == NULL || *p_stack == NULL) {
        return;
    }

    Stack* stack = *p_stack;
    Node* current = stack->top;

    // 遍历链表，释放所有节点及数据
    while (current != NULL) {
        Node* temp = current;
        current = current->next;  // 移动到下一个节点
        free(temp->data);         // 释放节点存储的元素数据
        free(temp);               // 释放节点本身
    }

    // 释放栈管理结构
    free(stack);
    *p_stack = NULL;  // 置空外部指针，防止悬垂指针
}

/**
 * @brief 压栈操作的实现
 */
bool stack_push(Stack* stack, const void* element_data) {
    // 检查参数有效性
    if (stack == NULL || element_data == NULL) {
        return false;
    }

    // 1. 分配新节点
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        return false;  // 内存不足
    }

    // 2. 为节点分配数据存储内存（大小为 element_size）
    new_node->data = malloc(stack->element_size);
    if (new_node->data == NULL) {
        free(new_node);  // 回滚：释放已分配的节点
        return false;    // 内存不足
    }

    // 3. 拷贝用户数据到节点（保证类型无关性）
    memcpy(new_node->data, element_data, stack->element_size);

    // 4. 将新节点插入栈顶
    new_node->next = stack->top;  // 新节点的下一个是原栈顶
    stack->top = new_node;        // 更新栈顶为新节点

    // 5. 更新栈大小
    stack->size++;
    return true;
}

/**
 * @brief 弹栈操作的实现
 */
bool stack_pop(Stack* stack, void* output_buffer) {
    // 检查参数有效性和栈状态
    if (stack_is_empty(stack) || output_buffer == NULL) {
        return false;
    }

    // 1. 暂存栈顶节点（待删除）
    Node* node_to_remove = stack->top;

    // 2. 拷贝数据到用户缓冲区
    memcpy(output_buffer, node_to_remove->data, stack->element_size);

    // 3. 更新栈顶指针
    stack->top = node_to_remove->next;

    // 4. 释放节点资源
    free(node_to_remove->data);  // 释放元素数据
    free(node_to_remove);        // 释放节点

    // 5. 更新栈大小
    stack->size--;
    return true;
}

/**
 * @brief 查看栈顶元素的实现
 */
bool stack_peek(const Stack* stack, void* output_buffer) {
    // 检查参数有效性和栈状态
    if (stack_is_empty(stack) || output_buffer == NULL) {
        return false;
    }

    // 仅拷贝数据，不修改栈结构
    memcpy(output_buffer, stack->top->data, stack->element_size);
    return true;
}

/**
 * @brief 检查栈是否为空的实现
 */
bool stack_is_empty(const Stack* stack) {
    // 空指针视为空栈
    if (stack == NULL) {
        return true;
    }
    // 栈顶为 NULL 或 size 为 0 均表示空栈（双重校验）
    return (stack->top == NULL) || (stack->size == 0);
}

/**
 * @brief 获取栈大小的实现
 */
size_t stack_get_size(const Stack* stack) {
    // 空指针返回 0
    if (stack == NULL) {
        return 0;
    }
    return stack->size;
}

