#ifndef NODE_H
#define NODE_H

#include <stddef.h> // 包含 size_t 定義

// --------------------------
// 1. 學生結構體聲明（供測試使用）
// --------------------------
typedef struct student
{
    int id;        // 學號
    char name[50]; // 姓名（用陣列避免動態記憶體，簡化測試）
    int age;       // 年齡
} student;

// --------------------------
// 2. 鏈表節點相關類型定義
// --------------------------
// 資料類型：此處直接使用 student（若需泛型，可改為
// void*，需調整記憶體管理邏輯）
typedef student data_type;

// 鏈表節點結構體
typedef struct node
{
    data_type data;    // 節點存儲的資料
    struct node *next; // 指向後一個節點的指標
} node;

// --------------------------
// 3. 鏈表操作函數聲明
// --------------------------
// 建立節點
node *create_node(data_type data);

// 尾部追加節點
void append_node(node **head_ref, data_type data);

// 頭部插入節點
void prepend_node(node **head_ref, data_type data);

// 指定索引插入節點（索引從0開始）
void insert_node_at(node **head_ref, size_t index, data_type data);

// 指定節點後插入節點
void insert_after_node(node *prev_node, data_type data);

// 列印鏈表（需傳入資料列印回呼）
void print_list(node *head, void (*print_func)(const void *data));

// 查找節點（需傳入資料比較回呼）
node *find_node(node *head, const void *target_data,
                int (*compare_func)(const void *a, const void *b,
                                    void *context),
                void *context);

// 刪除節點（需傳入資料比較回呼）
void delete_node(node **head_ref, const void *target_data,
                 int (*compare_func)(const void *a, const void *b,
                                     void *context),
                 void *context);

// 更新節點（需傳入資料比較回呼）
void update_node(node **head_ref, const void *target_data, data_type new_data,
                 int (*compare_func)(const void *a, const void *b,
                                     void *context),
                 void *context);

// 釋放鏈表（需傳入資料釋放回呼，可為NULL）
void free_list(node **head_ref, void (*free_data_func)(void *data));

#endif // NODE_H

