#pragma once

#include <stdbool.h> // 用於bool類型 (C99標準)
#include <stddef.h>  // 用於size_t

/**
 * @file sequentail_stakc.h
 * @brief 通過順序棧的ADT (抽象數據類型) 接口
 * @deteails 實現了一個線程不安全的通用順序棧, 支持任意類型元素的存儲,
 * 內部通過連續內存塊 (數組) 實現, 提供基本的棧操作接口
 */

// 不透明指針聲明: 用戶僅通過API操作, 無法直接訪問內部結構
typedef struct Stack Stack;

/**
 * @brief 創建新的順序棧
 * @param capacity 棧的最大容量 (元素數量)
 * @param element_size 每個元素的字節大小 (例如: sizeof(int))
 * @return 成功返回棧指針, 失敗 (參數無效或內存不足) 返回NULL
 * @note 當 capacity 或 element_size 爲 0 時， 返回NULL
 */
Stack *stack_create(size_t capacity, size_t element_size);

/**
 * @brief 銷燬並釋放所有內存
 * @param p_stack 指向棧指針的指針 (二級指針)
 * @note 函數執行後會將 *p_stack 置爲NULL, 防止指針懸掛
 */
void stack_destroy(Stack **p_stack);

/**
 * @brief 壓棧操作 (向棧頂添加元素)
 * @param stack 棧指針
 * @param element_data 待壓入元素的地址 (數據會被複製到棧內)
 * @return 成功返回true, 失敗 (棧滿或參數無效) 返回false
 */
bool stack_push(Stack *stack, const void *element_data);

/**
 * @brief 彈棧操作 (從棧頂移除元素)
 * @param stack 棧指針
 * @param output_buffer 接收彈出元素的緩衝區 (大小需要滿足 >= element_size)
 * @return 成功返回true, 失敗 (棧空或參數無效) 返回false
 */
void stack_pop(Stack *stack, void *output_buffer);

/**
 * @brief 查看棧頂元素 (不彈出)
 * @param stack 棧指針
 * @param output_buffer 接收彈出元素的緩衝區 (大小需要滿足 >= element_size)
 * @return 成功返回true, 失敗 (棧空或參數無效) 返回false
 */
bool stack_peek(const Stack *stack, void *output_buffer);

/**
 * @brief 檢查棧是否爲空
 * @param stack 棧指針
 * @return 棧爲空或satck爲NULL時返回true, 否則返回false
 */
bool stack_is_empty(const Stack *stack);

/**
 * @brief 檢查棧是否已滿
 * @param stack 棧指針
 * @return 棧已滿返回true, 否則(包括stack爲NULL) 返回false
 */
bool stack_is_full(const Stack *stack);

/**
 * @brief 獲取當前棧中元素數量
 * @param stack 棧指針
 * @return 元素數量 (stack爲NULL時返回0)
 */
size_t stack_get_size(const Stack *stack);

/**
 * @brief 獲取棧的總容量
 * @param stack 棧指針
 * @return 總容量 (stack爲NULL時返回0)
 */
size_t stack_get_capacity(const Stack *stack);

