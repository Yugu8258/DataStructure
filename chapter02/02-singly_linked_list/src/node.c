#include "node.h"

#include <stdint.h> // 包含 SIZE_MAX 定義
#include <stdio.h>
#include <stdlib.h>

/**
 * 建立新節點
 * @param data 節點存儲的資料
 * @return 成功返回節點指標，失敗返回NULL
 */
node *create_node(data_type data)
{
    // 使用 sizeof(*new_node) 確保類型修改時的健壯性
    node *new_node = (node *)malloc(sizeof(*new_node));
    if (!new_node)
    {
        perror("建立節點失敗: 記憶體分配錯誤!");
        return NULL;
    }

    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

/**
 * 尾部追加節點
 * @param head_ref 表頭指標的位址（二級指標，確保能修改表頭）
 * @param data 要追加的資料
 */
void append_node(node **head_ref, data_type data)
{
    if (!head_ref)
    {
        fprintf(stderr, "錯誤: append_node - head_ref不能為空指標!\n");
        return;
    }

    node *new_node = create_node(data);
    if (!new_node)
        return;

    // 空鏈表：直接讓表頭指向新節點
    if (!*head_ref)
    {
        *head_ref = new_node;
        return;
    }

    // 遍歷到鏈表尾部
    node *last = *head_ref;
    while (last->next)
    {
        last = last->next;
    }
    last->next = new_node;
}

/**
 * 頭部插入節點
 * @param head_ref 表頭指標的位址
 * @param data 要插入的資料
 */
void prepend_node(node **head_ref, data_type data)
{
    if (!head_ref)
    {
        fprintf(stderr, "錯誤: prepend_node - head_ref不能為空指標!\n");
        return;
    }

    node *new_node = create_node(data);
    if (!new_node)
        return;

    // 新節點指向原表頭，再更新表頭
    new_node->next = *head_ref;
    *head_ref = new_node;
}

/**
 * 指定索引插入節點（索引從0開始）
 * @param head_ref 表頭指標的位址
 * @param index 插入位置
 * @param data 要插入的資料
 */
void insert_node_at(node **head_ref, size_t index, data_type data)
{
    if (!head_ref || index > SIZE_MAX)
    {
        fprintf(stderr, "錯誤: insert_node_at - 無效的索引或頭指標!\n");
        return;
    }

    node *new_node = create_node(data);
    if (!new_node)
        return;

    // 插入到頭部（索引0）
    if (index == 0)
    {
        new_node->next = *head_ref;
        *head_ref = new_node;
        printf("資訊: 已在位置 %zu 插入節點\n", index);
        return;
    }

    // 找到插入位置的前一個節點
    node *current = *head_ref;
    size_t current_index = 0;
    while (current && current_index < index - 1)
    {
        current = current->next;
        current_index++;
    }

    // 索引超出鏈表長度：追加到尾部
    if (!current)
    {
        printf("警告: insert_node_at - 索引 %zu 超出鏈表長度, 已追加到末尾\n",
               index);
        if (!*head_ref)
        {
            *head_ref = new_node;
        }
        else
        {
            node *last = *head_ref;
            while (last->next)
                last = last->next;
            last->next = new_node;
        }
        return;
    }

    // 正常插入：新節點接後續，前節點接新節點
    new_node->next = current->next;
    current->next = new_node;
    printf("資訊: 已在位置 %zu 插入節點\n", index);
}

/**
 * 指定節點後插入新節點
 * @param prev_node 前置節點（必須非空）
 * @param data 要插入的資料
 */
void insert_after_node(node *prev_node, data_type data)
{
    if (!prev_node)
    {
        fprintf(stderr, "錯誤: insert_after_node - 前置節點不能為空!\n");
        return;
    }

    node *new_node = create_node(data);
    if (!new_node)
        return;

    // 新節點接前置節點的後續，前置節點接新節點
    new_node->next = prev_node->next;
    prev_node->next = new_node;
}

/**
 * 列印鏈表
 * @param head 表頭指標
 * @param print_func 資料列印回呼（需自定義資料格式）
 */
void print_list(node *head, void (*print_func)(const void *data))
{
    if (!print_func)
    {
        fprintf(stderr, "錯誤: print_list - 未提供列印函數!\n");
        return;
    }

    printf("鏈表內容: ");
    node *current = head;
    while (current)
    {
        print_func(&current->data); // 調用回呼列印資料
        printf(" -> ");
        current = current->next;
    }
    printf("NULL\n");
}

/**
 * 查找節點（核心修復：比較函數判斷邏輯）
 * @param head 表頭指標
 * @param target_data 目標資料（用於比較）
 * @param compare_func 資料比較回呼（返回0表示匹配）
 * @param context 比較函數的上下文（可選，可為NULL）
 * @return 找到返回節點指標，未找到返回NULL
 */
node *find_node(node *head, const void *target_data,
                int (*compare_func)(const void *a, const void *b,
                                    void *context),
                void *context)
{
    // 核心修復：比較函數為空時才報錯（原邏輯顛倒）
    if (!compare_func)
    {
        fprintf(stderr, "錯誤: find_node - 未提供比較函數!\n");
        return NULL;
    }

    node *current = head;
    while (current)
    {
        // 調用回呼比較當前節點與目標資料
        if (compare_func(&current->data, target_data, context) == 0)
        {
            return current; // 匹配：返回當前節點
        }
        current = current->next;
    }
    return NULL; // 未找到
}

/**
 * 刪除節點
 * @param head_ref 表頭指標的位址
 * @param target_data 目標資料（用於比較）
 * @param compare_func 資料比較回呼
 * @param context 比較函數的上下文（可選）
 */
void delete_node(node **head_ref, const void *target_data,
                 int (*compare_func)(const void *a, const void *b,
                                     void *context),
                 void *context)
{
    if (!head_ref || !*head_ref || !compare_func)
    {
        fprintf(stderr, "錯誤: delete_node - 無效參數或空鏈表!\n");
        return;
    }

    node *temp = *head_ref;
    node *prev = NULL;

    // 情況1：刪除表頭節點
    if (compare_func(&temp->data, target_data, context) == 0)
    {
        *head_ref = temp->next; // 表頭指向後續節點
        free(temp);             // 釋放原表頭
        printf("資訊: delete_node - 頭節點已經刪除!\n");
        return;
    }

    // 情況2：刪除非表頭節點（遍歷查找）
    while (temp && compare_func(&temp->data, target_data, context) != 0)
    {
        prev = temp;       // 記錄前一個節點
        temp = temp->next; // 移動到下一個節點
    }

    // 未找到目標節點
    if (!temp)
    {
        printf("警告: delete_node - 未找到目標節點, 無法刪除!\n");
        return;
    }

    // 找到：前節點跳過當前節點，釋放當前節點
    prev->next = temp->next;
    free(temp);
    printf("資訊: delete_node - 節點已經刪除!\n");
}

/**
 * 更新節點資料
 * @param head_ref 表頭指標的位址
 * @param target_data 目標資料（用於查找）
 * @param new_data 新資料（用於更新）
 * @param compare_func 資料比較回呼
 * @param context 比較函數的上下文（可選）
 */
void update_node(node **head_ref, const void *target_data, data_type new_data,
                 int (*compare_func)(const void *a, const void *b,
                                     void *context),
                 void *context)
{
    if (!head_ref || !*head_ref || !compare_func)
    {
        fprintf(stderr, "錯誤: update_node - 無效參數或空鏈表!\n");
        return;
    }

    // 先查找目標節點
    node *node_to_update =
        find_node(*head_ref, target_data, compare_func, context);
    if (node_to_update)
    {
        node_to_update->data = new_data; // 更新資料
        printf("資訊: update_node - 節點已經成功更新!\n");
    }
    else
    {
        printf("警告: update_node - 未找到目標節點, 無法更新!\n");
    }
}

/**
 * 釋放鏈表（核心修復：避免訪問已釋放記憶體）
 * @param head_ref 表頭指標的位址
 * @param free_data_func 資料釋放回呼（可選，用於釋放動態記憶體）
 */
void free_list(node **head_ref, void (*free_data_func)(void *data))
{
    if (!head_ref)
        return;

    node *current = *head_ref;
    node *next_node;

    while (current)
    {
        next_node = current->next; // 先保存下一個節點位址（核心修復）
        // 若有資料釋放回呼，先釋放資料（此處student無動態記憶體，可忽略）
        if (free_data_func)
        {
            free_data_func(&current->data);
        }
        free(current);       // 釋放當前節點
        current = next_node; // 使用保存的位址移動（原錯誤：current->next）
    }

    *head_ref = NULL; // 表頭置空，避免野指標
}

