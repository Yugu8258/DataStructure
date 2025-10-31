#pragma once

#include <stdbool.h>
#include <stddef.h>

/**
 * @file circular_doubly_linked_list.h
 * @brief 線程安全的循環雙向鏈表實現 (基於內存池管理節點)
 * @details 鏈表首尾相連, 支持向前/向後旋轉, 適用於音樂播放列表等場景
 */

// 向前聲明 (隱藏內部實現)
typedef struct circular_doubly_linked_list circular_doubly_linked_list;
typedef struct circular_doubly_list_node circular_doubly_list_node;

/**
 * @brief 數據釋放回調函數類型
 * @param data 待釋放的數據指針
 */
typedef void (*free_func)(void *);

/**
 * @brief 數據比較回調函數類型
 * @param data1 第一個比較函數
 * @param data2 第二個比較函數
 */
typedef int (*compare_func)(const void *, const void *);

/**
 * @brief 遍歷操作回調函數類型
 * @param data 節點數據
 * @param context 外部傳入的上下文參數
 */
typedef void (*action_func)(void *, void *);

/**
 * @brief 創建循環雙向鏈表
 * @param initial_capacity 內存池初始容量 (預分配節點數量)
 * @param free_func 數據釋放回調函數 (NULL表示不自動釋放數據)
 * @return 鏈表指針, 失敗返回NULL
 */
circular_doubly_linked_list *list_create(size_t initial_capacity,
                                         free_func free_function);

/**
 * @brief 銷燬循環雙向鏈表
 * @param list_ptr 鏈表指針的指針 (銷燬後會被置爲NULL)
 */
void list_destroy(circular_doubly_linked_list **list_ptr);

/**
 * @brief 在鏈表尾部追加節點
 * @param list 鏈表指針
 * @param data 待存儲的數據 (需要確保生命週期合法)
 * @return 成功返回true, 失敗返回 false
 */
bool list_append(circular_doubly_linked_list *list, void *data);

/**
 * @brief 在鏈表頭部插入節點
 * @param list 鏈表指針
 * @param data 待存儲的數據
 * @return 成功返回true, 失敗返回false
 */
bool list_prepend(circular_doubly_linked_list *list, void *data);

/**
 * @brief 刪除節點操作
 * @param list 鏈表指針
 * @param node 待刪除的節點 (必須屬於當前鏈表)
 */
void list_delete_node(circular_doubly_linked_list *list,
                      circular_doubly_list_node *node);

/**
 * @brief 查找節點 (基於比較函數)
 * @param list 鏈表指針
 * @param data_to_find 查找關鍵字
 * @param compare_func 比較函數
 * @return 找到的節點指針, 未找到返回NULL
 */
circular_doubly_list_node *list_find_node(circular_doubly_linked_list *list,
                                          const void *data_to_find,
                                          compare_func compare_function);

/**
 * @brief 向前旋轉鏈表 (頭結點後移一位)
 * @param list 鏈表指針 (節點數需>=2才有效)
 */
void list_rotate_forward(circular_doubly_linked_list *list);

/**
 * @brief 向後旋轉鏈表 (頭結點前移一位)
 * @param list 鏈表指針 (節點數需>=2才有效)
 */
void list_rotate_backward(circular_doubly_linked_list *list);

/**
 * @brief 獲取鏈表節點數量
 * @param list 鏈表指針
 * @return 節點數量, 鏈表爲NULL時返回0
 */
size_t list_get_size(const circular_doubly_linked_list *list);

/**
 * @brief 遍歷鏈表並執行操作
 * @param list 鏈表指針
 * @param action_function 對每個節點執行的操作函數
 * @param context 傳遞給操作函數的上下文參數
 */
void list_for_each(circular_doubly_linked_list *list,
                   action_func action_function, void *context);

/**
 * @brief 獲取節點存儲的數據
 * @param node 節點指針
 * @return 數據指針, 節點爲NULL時返回NULL
 */
void *circular_doubly_list_node_get_data(const circular_doubly_list_node *node);

/**
 * @brief 獲取鏈表當前頭節點
 * @param list 鏈表指針
 * @return 頭結點指針, 鏈表爲空時返回NULL
 */
circular_doubly_list_node *list_get_head_node(
    circular_doubly_linked_list *list);

