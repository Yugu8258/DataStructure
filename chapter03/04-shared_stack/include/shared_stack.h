/**
 * @file shared_stack.h
 * @brief 共享栈（Shared Stack）的 ADT 接口定义
 * 用一个数组实现两个栈，栈底分别在数组两端，向中间生长，节省内存空间
 */
#ifndef SHARED_STACK_H
#define SHARED_STACK_H

#include <stdbool.h> // 提供 bool 类型
#include <stddef.h>  // 提供 size_t 类型

// 栈标识：区分两个栈
typedef enum
{
    STACK_A, // 栈 A（从数组左侧开始）
    STACK_B  // 栈 B（从数组右侧开始）
} StackID;

// 共享栈的 opaque 类型（隐藏内部实现）
typedef struct SharedStack SharedStack;

/**
 * @brief 创建一个共享栈
 * @param capacity 数组总容量（两个栈共享的最大元素数量）
 * @param elem_size 每个元素的大小（字节），例如 sizeof(int)
 * @return 成功返回共享栈指针，失败（capacity=0 或 elem_size=0 或内存不足）返回
 * NULL
 */
SharedStack *shared_stack_create(size_t capacity, size_t elem_size);

/**
 * @brief 销毁共享栈并释放内存
 * @param stack 指向共享栈指针的指针（双重指针确保外部指针置空）
 */
void shared_stack_destroy(SharedStack **stack);

/**
 * @brief 向指定栈压入元素
 * @param stack 共享栈指针（不可为 NULL）
 * @param id 栈标识（STACK_A 或 STACK_B）
 * @param elem 指向待压入元素的指针（不可为 NULL）
 * @return 成功返回 true，失败（栈满或参数无效）返回 false
 */
bool shared_stack_push(SharedStack *stack, StackID id, const void *elem);

/**
 * @brief 从指定栈弹出元素
 * @param stack 共享栈指针（不可为 NULL）
 * @param id 栈标识（STACK_A 或 STACK_B）
 * @param elem 用于接收弹出元素的缓冲区（不可为 NULL，需提前分配内存）
 * @return 成功返回 true，失败（栈空或参数无效）返回 false
 */
bool shared_stack_pop(SharedStack *stack, StackID id, void *elem);

/**
 * @brief 查看指定栈的栈顶元素（不弹出）
 * @param stack 共享栈指针（不可为 NULL）
 * @param id 栈标识（STACK_A 或 STACK_B）
 * @param elem 用于接收栈顶元素的缓冲区（不可为 NULL，需提前分配内存）
 * @return 成功返回 true，失败（栈空或参数无效）返回 false
 */
bool shared_stack_peek(const SharedStack *stack, StackID id, void *elem);

/**
 * @brief 检查指定栈是否为空
 * @param stack 共享栈指针（不可为 NULL）
 * @param id 栈标识（STACK_A 或 STACK_B）
 * @return 栈为空返回 true，否则返回 false
 */
bool shared_stack_is_empty(const SharedStack *stack, StackID id);

/**
 * @brief 检查指定栈是否已满（整个共享栈满时返回 true）
 * @param stack 共享栈指针（不可为 NULL）
 * @return 共享栈已满返回 true，否则返回 false
 */
bool shared_stack_is_full(const SharedStack *stack);

/**
 * @brief 获取指定栈的当前元素数量
 * @param stack 共享栈指针（不可为 NULL）
 * @param id 栈标识（STACK_A 或 STACK_B）
 * @return 栈中元素的个数
 */
size_t shared_stack_size(const SharedStack *stack, StackID id);

/**
 * @brief 清空指定栈（仅清空目标栈，不影响另一个栈）
 * @param stack 共享栈指针（不可为 NULL）
 * @param id 栈标识（STACK_A 或 STACK_B）
 */
void shared_stack_clear(SharedStack *stack, StackID id);

#endif // SHARED_STACK_H

