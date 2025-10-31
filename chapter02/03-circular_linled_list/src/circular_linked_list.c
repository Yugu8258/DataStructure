#include "circular_linked_list.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// 建立新節點
node *create_node(data_type data)
{
    node *new_node = (node *)malloc(sizeof(*new_node));
    if (!new_node)
    {
        perror("建立節點失敗：記憶體分配錯誤");
        return NULL;
    }

    new_node->data = data;
    new_node->next = NULL; // 暫時指向 NULL，插入時調整為循環
    return new_node;
}

// 尾部追加節點（循環鏈表核心：最後一個節點指向表頭）
void append_node(node **head_ref, data_type data)
{
    if (!head_ref)
    {
        fprintf(stderr, "錯誤：append_node - head_ref 不能為空指標\n");
        return;
    }

    node *new_node = create_node(data);
    if (!new_node)
        return;

    // 情況 1：空鏈表（新節點指向自己，成為循環）
    if (!*head_ref)
    {
        *head_ref = new_node;
        new_node->next = *head_ref; // 最後一個節點（唯一節點）指向表頭
        return;
    }

    // 情況 2：非空鏈表（找到最後一個節點，調整指標）
    node *last = *head_ref;
    // 循環終止條件：last->next == *head_ref（回到表頭，即找到最後一個節點）
    while (last->next != *head_ref)
    {
        last = last->next;
    }

    // 新節點接在最後，並指向表頭；原最後一個節點指向新節點
    new_node->next = *head_ref;
    last->next = new_node;
}

// 头部插入節點（循環鏈表核心：原最後一個節點指向新表頭）
void prepend_node(node **head_ref, data_type data)
{
    if (!head_ref)
    {
        fprintf(stderr, "錯誤：prepend_node - head_ref 不能為空指標\n");
        return;
    }

    node *new_node = create_node(data);
    if (!new_node)
        return;

    // 情況 1：空鏈表（新節點指向自己）
    if (!*head_ref)
    {
        *head_ref = new_node;
        new_node->next = *head_ref;
        return;
    }

    // 情況 2：非空鏈表（先找到最後一個節點，再調整指標）
    node *last = *head_ref;
    while (last->next != *head_ref)
    {
        last = last->next;
    }

    // 新節點指向原表頭，原最後一個節點指向新節點，更新表頭
    new_node->next = *head_ref;
    last->next = new_node;
    *head_ref = new_node;
}

// 列印循環鏈表（避免無限迴圈：從表頭開始，回到表頭即停止）
void print_list(node *head, void (*print_func)(const void *data))
{
    if (!print_func)
    {
        fprintf(stderr, "錯誤：print_list - 未提供列印函數\n");
        return;
    }
    if (!head)
    {
        printf("鏈表內容：NULL（空鏈表）\n");
        return;
    }

    printf("鏈表內容：");
    node *current = head;
    // 循環終止條件：current->next == head（回到表頭，所有節點已列印）
    do
    {
        print_func(&current->data);
        printf(" -> ");
        current = current->next;
    } while (current != head);

    printf("（回到表頭，循環結束）\n");
}

// 查詢節點（循環鏈表：從表頭開始，回到表頭即停止查詢）
node *find_node(node *head, const void *target_data,
                int (*compare_func)(const void *a, const void *b,
                                    void *context),
                void *context)
{
    if (!compare_func)
    {
        fprintf(stderr, "錯誤：find_node - 未提供比較函數\n");
        return NULL;
    }
    if (!head)
    {
        fprintf(stderr, "錯誤：find_node - 空鏈表，無法查詢\n");
        return NULL;
    }

    node *current = head;
    do
    {
        // 調用回呼函數比較，返回 0 表示匹配
        if (compare_func(&current->data, target_data, context) == 0)
        {
            return current;
        }
        current = current->next;
    } while (current != head);

    return NULL; // 遍歷結束未找到
}

// 刪除節點（循環鏈表核心：處理表頭/非表頭/唯一節點三種情況）
void delete_node(node **head_ref, const void *target_data,
                 int (*compare_func)(const void *a, const void *b,
                                     void *context),
                 void *context)
{
    if (!head_ref || !*head_ref || !compare_func)
    {
        fprintf(stderr, "錯誤：delete_node - 無效參數或空鏈表\n");
        return;
    }

    node *current = *head_ref;
    node *prev = NULL;

    // 步驟 1：找到目標節點（current）和其前一個節點（prev）
    do
    {
        if (compare_func(&current->data, target_data, context) == 0)
        {
            break; // 找到目標節點，跳出循環
        }
        prev = current;
        current = current->next;
    } while (current != *head_ref);

    // 步驟 2：判斷是否找到目標節點
    if (current == *head_ref && !prev)
    {
        // 子情況 2.1：未找到目標節點（遍歷一圈回到表頭，prev 仍為 NULL）
        printf("警告：delete_node - 未找到目標節點，無法刪除\n");
        return;
    }

    // 步驟 3：處理不同刪除場景
    // 場景 A：鏈表只有一個節點（刪除後變空鏈表）
    if (current->next == current)
    {
        free(current);
        *head_ref = NULL;
        printf("資訊：delete_node - 唯一節點已刪除，鏈表現為空\n");
        return;
    }

    // 場景 B：刪除的是表頭節點
    if (current == *head_ref)
    {
        // 先找到最後一個節點，讓其指向新表頭
        node *last = *head_ref;
        while (last->next != *head_ref)
        {
            last = last->next;
        }
        last->next = current->next; // 最後一個節點指向原表頭的下一個節點
        *head_ref = current->next;  // 更新表頭
        free(current);
        printf("資訊：delete_node - 表頭節點已刪除\n");
        return;
    }

    // 場景 C：刪除的是非表頭節點
    prev->next = current->next; // 前一個節點跳過當前節點
    free(current);
    printf("資訊：delete_node - 非表頭節點已刪除\n");
}

// 釋放循環鏈表所有記憶體（避免無限迴圈：從表頭開始，釋放至原最後一個節點）
void free_list(node **head_ref, void (*free_data_func)(void *data))
{
    if (!head_ref || !*head_ref)
        return;

    node *current = *head_ref;
    node *next_node;
    // 循環終止條件：current->next == *head_ref（回到表頭，最後一個節點）
    do
    {
        next_node = current->next; // 先保存下一個節點位址
        // 若有資料釋放回呼，先釋放資料內部資源
        if (free_data_func)
        {
            free_data_func(&current->data);
        }
        free(current); // 釋放當前節點
        current = next_node;
    } while (current != *head_ref);

    *head_ref = NULL; // 表頭置空，避免野指標
    printf("資訊：free_list - 循環鏈表所有記憶體已釋放\n");
}

