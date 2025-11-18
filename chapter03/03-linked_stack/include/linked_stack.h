#pragma once

/**
 * @file linked_stack.h
 * @brief 通用鏈式棧的接口定義
 * 實現一個類型無關的鏈式棧, 支持任意數據類型的壓棧, 彈棧等操作
 */

#include <stdbool.h> // 提供 bool 類型
#include <stddef.h>  // 提供 size_t 類型

// opaque 指針: 隱藏棧的內部實現, 用戶無需關心具體結構
typedef struct LinkedStack Stack;

/**
 * @brief 创建一个新的链式栈
 * @param element_size 栈中每个元素的大小（字节），例如 sizeof(int)
 * @return 成功返回栈指针，失败（element_size=0 或内存不足）返回 NULL
 */
Stack *stack_create(size_t element_size);

/**
 * @brief 销毁栈并释放所有内存
 * @param p_stack 指向栈指针的指针（双重指针确保外部指针能被置空）
 * @note 调用后 *p_stack 将被设置为 NULL，防止悬垂指针
 */
void stack_destroy(Stack **p_stack);

/**
 * @brief 将元素压入栈顶
 * @param stack 栈指针（不可为 NULL）
 * @param element_data 指向待压入元素的指针（不可为 NULL）
 * @return 成功返回 true，失败（参数无效或内存不足）返回 false
 */
bool stack_push(Stack *stack, const void *element_data);

/**
 * @brief 从栈顶弹出元素
 * @param stack 栈指针（不可为 NULL）
 * @param output_buffer 接收弹出元素的缓冲区（不可为 NULL，需提前分配内存）
 * @return 成功返回 true，失败（栈为空或参数无效）返回 false
 */
bool stack_pop(Stack *stack, void *output_buffer);

/**
 * @brief 查看栈顶元素（不弹出）
 * @param stack 栈指针（不可为 NULL）
 * @param output_buffer 接收栈顶元素的缓冲区（不可为 NULL，需提前分配内存）
 * @return 成功返回 true，失败（栈为空或参数无效）返回 false
 */
bool stack_peek(const Stack *stack, void *output_buffer);

/**
 * @brief 检查栈是否为空
 * @param stack 栈指针（可为 NULL，视为空栈）
 * @return 栈为空返回 true，否则返回 false
 */
bool stack_is_empty(const Stack *stack);

/**
 * @brief 获取栈中元素数量
 * @param stack 栈指针（可为 NULL，返回 0）
 * @return 栈中元素个数
 */
size_t stack_get_size(const Stack *stack);

