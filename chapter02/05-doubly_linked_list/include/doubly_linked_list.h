#pragma once

#include <stdbool.h>
#include <stddef.h>

/**
 * @file boubly_linked_list.h
 * @brief 線程安全的雙向鏈表實現, 基於內存池管理節點, 支持自定義數據操作
 * @details 採用Opaque Pointer模式封裝內部結構, 用戶僅通過API訪問, 確保封裝性
 */

// 向前聲明 (Opaque Pointer, 硬藏內部實現)
typedef struct doubly_linked_list doubly_linked_list;
typedef struct doubly_list_node doubly_list_node;

/**
 * @brief 數據比較函數指針類型
 * @param data1 第一個比餃數據
 * @param data2 第二個比較數據
 * @return 0表示相等, 正數表示data1>data2, 負數表示data1<data2
 */
typedef int (*compare_func)(const void *data1, const void *data2);

/**
 * @brief 數據打印函數指針類型 (用於遍歷打印)
 * @param data 需要打印的數據
 */
typedef void (*print_func)(const void *data);

/**
 * @brief 數據釋放函數指針類型 (用戶自定義數據的釋放邏輯)
 * @param data 需要釋放的數據
 */
typedef void (*free_func)(void *data);

/**
 * @brief 遍歷操作函數指針類型 (對每個節點數據執行自定義操作)
 * @param data 節點數據
 * @param context 外部傳入的上下文參數
 */
typedef void (*action_func)(void *data, void *context);

/**
 * @brief 創建雙向鏈表
 * @param initial_capacity 內存池初始容量 (預分配節點數量, 建議更具預期規模設置)
 * @param free_function 數據釋放回調函數 (NULL表示不自動釋放數據)
 * @return 若initial_capacity爲0, 會默認創建10個節點的內存池
 */
doubly_linked_list *list_create(size_t initial_capacity,
                                free_func free_function);

/**
 * @brief 銷燬雙向鏈表(釋放所有資源)
 * @param list_ptr 鏈表指針的指針 (銷燬後會被置爲NULL, 避免野指針)
 */
void list_destroy(doubly_linked_list **list_ptr);

/**
 * @brief 在鏈表尾部追加節點
 * @param list 鏈表指針
 * @param data 要存儲的數據 (需要確保數據生命週期合法)
 * @return 成功返回true, 失敗返回false (內存池消耗殆盡或無效參數)
 */
bool list_append(doubly_linked_list *list, const void *data);

/**
 * @brief 在鏈表頭部插入節點
 * @param list 鏈表指針
 * @param data 要存儲的數據
 * @return 成功返回true, 失敗返回false
 */
bool list_prepend(doubly_linked_list *list, const void *data);

/**
 * @brief 在指定節點後插入新節點
 * @param list 鏈表指針
 * @param node 目標節點(必須屬於當前鏈表)
 * @param data 要存儲的數據
 * @return 成功返回true, 失敗返回false
 */
bool list_insert_after(doubly_linked_list *list, doubly_list_node *node,
                       const void *data);

/**
 * @brief 刪除指定節點
 * @param list 鏈表指針
 * @param node 要刪除的節點 (必須屬於當前節點)
 * @note 會自動調用用戶傳入的free_func釋放節點數據
 */
void list_delete_node(doubly_linked_list *list, doubly_list_node *node);

/**
 * @brief 查找節點 (基於比較函數)
 * @param list 鏈表指針
 * @param data_to_find 查找關鍵字 (需要比較函數匹配)
 * @param compare_function 比較函數
 * @return 找到的節點指針, 未找到返回NULL
 */
doubly_list_node *list_find(doubly_linked_list *list, const void *data_to_find,
                            compare_func compare_function);

/**
 * @brief 獲取鏈表當前節點數量
 * @param list 鏈表指針
 * @return 節點數量, 鏈表爲NULL時返回0
 */
size_t list_get_size(const doubly_linked_list *list);

/**
 * @brief 遍歷鏈表並對每個節點執行操作
 * @param list 鏈表指針
 * @param action_function 對每個節點執行的操作函數
 * @param context 傳遞給操作函數的上下文指針(可爲NULL)
 */
void list_for_each(doubly_linked_list *list, action_func action_function,
                   void *context);

/**
 * @brief 獲取節點存儲的數據
 * @param node 節點指針
 * @return 數據指針, 節點爲NULL時返回NULL
 */
void *list_get_data(const doubly_list_node *node);

