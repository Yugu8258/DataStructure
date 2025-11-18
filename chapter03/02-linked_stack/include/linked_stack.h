/**
 * @file linked_stack.h
 * @brief 链式栈（Linked Stack）的 ADT 接口定义
 * 支持任意数据类型的压栈、弹栈、查看栈顶等操作，内部基于链表实现
 */
#ifndef LINKED_STACK_H
#define LINKED_STACK_H

#include <stdbool.h> // 提供 bool 类型（C99 标准）
#include <stddef.h>  // 提供 size_t 类型

//  opaque 类型：隐藏栈的内部实现，用户仅通过指针操作
typedef struct StackNode Stack;

/**
 * @brief 创建一个新的链式栈
 * @param elem_size 栈中每个元素的大小（单位：字节），例如 sizeof(int)
 * @return 成功返回栈的指针，失败（elem_size=0 或内存不足）返回 NULL
 */
Stack *stack_create(size_t elem_size);

/**
 * @brief 销毁栈并释放所有内存
 * @param stack 指向栈指针的指针（双重指针确保外部指针被置空）
 * @note 调用后，*stack 将被设置为 NULL，防止悬垂指针
 */
void stack_destroy(Stack **stack);

/**
 * @brief 将元素压入栈顶
 * @param stack 栈的指针（不可为 NULL）
 * @param elem 指向待压入元素的指针（不可为 NULL，需保证内存有效）
 * @return 成功返回 true，失败（参数无效或内存不足）返回 false
 */
bool stack_push(Stack *stack, const void *elem);

/**
 * @brief 从栈顶弹出元素
 * @param stack 栈的指针（不可为 NULL）
 * @param elem 用于接收弹出元素的缓冲区（不可为 NULL，需提前分配内存）
 * @return 成功返回 true，失败（栈为空或参数无效）返回 false
 */
bool stack_pop(Stack *stack, void *elem);

/**
 * @brief 查看栈顶元素（不弹出）
 * @param stack 栈的指针（不可为 NULL）
 * @param elem 用于接收栈顶元素的缓冲区（不可为 NULL，需提前分配内存）
 * @return 成功返回 true，失败（栈为空或参数无效）返回 false
 */
bool stack_peek(const Stack *stack, void *elem);

/**
 * @brief 检查栈是否为空
 * @param stack 栈的指针（可为 NULL，视为空栈）
 * @return 栈为空返回 true，否则返回 false
 */
bool stack_is_empty(const Stack *stack);

/**
 * @brief 获取栈中元素的数量
 * @param stack 栈的指针（可为 NULL，返回 0）
 * @return 栈中元素的个数
 */
size_t stack_size(const Stack *stack);

/**
 * @brief 清空栈中所有元素（保留栈结构）
 * @param stack 栈的指针（不可为 NULL）
 */
void stack_clear(Stack *stack);

#endif // LINKED_STACK_H

