#ifndef CIRCULAR_LINKED_LIST_H
#define CIRCULAR_LINKED_LIST_H

#include <stddef.h> // 包含 size_t 定義

// --------------------------
// 學生結構體（測試用資料）
// --------------------------
typedef struct student
{
    int id;        // 學號
    char name[50]; // 姓名（陣列儲存，避免動態記憶體複雜度）
    int age;       // 年齡
} student;

// --------------------------
// 單向循環鏈表節點結構體
// --------------------------
// 資料型別：此處直接使用 student（可根據需求替換為其他型別）
typedef student data_type;

typedef struct node
{
    data_type data;    // 節點儲存的資料
    struct node *next; // 指向下一個節點的指標（最後一個節點指向表頭）
} node;

// --------------------------
// 循環鏈表操作函數宣告
// --------------------------
/**
 * 建立新節點
 * @param data 節點要儲存的資料
 * @return 成功返回節點指標，失敗返回 NULL
 */
node *create_node(data_type data);

/**
 * 尾部追加節點（循環鏈表：最後一個節點指向表頭）
 * @param head_ref 表頭指標的位址（二級指標，確保能修改表頭）
 * @param data 要追加的資料
 */
void append_node(node **head_ref, data_type data);

/**
 * 头部插入節點（循環鏈表：新節點指向原表頭，原最後一個節點指向新節點）
 * @param head_ref 表頭指標的位址
 * @param data 要插入的資料
 */
void prepend_node(node **head_ref, data_type data);

/**
 * 列印循環鏈表（避免無限迴圈：從表頭開始，回到表頭即停止）
 * @param head 表頭指標
 * @param print_func 資料列印回呼函數（自定義資料格式）
 */
void print_list(node *head, void (*print_func)(const void *data));

/**
 * 查詢節點（根據自定義條件）
 * @param head 表頭指標
 * @param target_data 目標資料（用於比較）
 * @param compare_func 資料比較回呼函數（返回 0 表示匹配）
 * @param context 比較函數的上下文（可傳入額外參數，如最小年齡）
 * @return 找到返回節點指標，未找到返回 NULL
 */
node *find_node(node *head, const void *target_data,
                int (*compare_func)(const void *a, const void *b,
                                    void *context),
                void *context);

/**
 * 刪除節點（根據自定義條件）
 * @param head_ref 表頭指標的位址
 * @param target_data 目標資料（用於比較）
 * @param compare_func 資料比較回呼函數
 * @param context 比較函數的上下文
 */
void delete_node(node **head_ref, const void *target_data,
                 int (*compare_func)(const void *a, const void *b,
                                     void *context),
                 void *context);

/**
 * 釋放循環鏈表所有記憶體（避免無限迴圈：從表頭開始，釋放至原最後一個節點）
 * @param head_ref 表頭指標的位址
 * @param free_data_func 資料釋放回呼函數（可釋放資料內部動態記憶體）
 */
void free_list(node **head_ref, void (*free_data_func)(void *data));

#endif // CIRCULAR_LINKED_LIST_H

