#include "doubly_linked_list.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * 內部輔助函數: 創建單個節點並初始化歌曲數據
 * @param song_data 要存入節點的歌曲數據
 * @return 成功返回節點指針, 失敗返回 NULL
 */
static node *_create_node(song song_data)
{
    // 防禦性檢查: 比賣弄空指針複製
    if (!song_data.title || !song_data.artist)
    {
        fprintf(stderr,
                "[_create_node] 錯誤: 歌曲標題/歌手名稱爲空, 無法創建節點\n");
        return NULL;
    }

    // 1. 分配節點內存
    node *new_node = (node *)malloc(sizeof(*new_node));
    if (!new_node)
    {
        perror("[_create_node] 節點內存分配失敗");
        return NULL;
    }

    // 2. 分配歌曲動態字符串內存 (+1 存儲 '\0' 結束符)
    new_node->data.title = (char *)malloc(strlen(song_data.title) + 1);
    new_node->data.artist = (char *)malloc(strlen(song_data.artist) + 1);

    // 3. 檢查字符串內存分配結果, 失敗則釋放已分配的資源
    if (!new_node->data.title || !new_node->data.artist)
    {
        perror("[_create_node] 歌曲字符串內存分配失敗");
        free(new_node->data.title); // 釋放已成功的部分
        free(new_node->data.artist);
        free(new_node);
        return NULL;
    }

    // 4. 複製歌曲數據到節點
    strcpy(new_node->data.title, song_data.title);
    strcpy(new_node->data.artist, song_data.artist);
    new_node->data.duration = song_data.duration;

    // 5. 初始化前後指針
    new_node->next = NULL;
    new_node->prev = NULL;

    return new_node;
}

doubly_linked_list *create_list(void)
{
    doubly_linked_list *list = (doubly_linked_list *)malloc(sizeof(*list));
    if (!list)
    {
        perror("[create_list] 鏈表管理器內存分配失敗!\n");
        return NULL;
    }

    // 初始化空鏈表狀態
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

    fprintf(stderr, "[create_list] 信息: 空雙向鏈表創建成功!\n");

    return list;
}

void free_list(doubly_linked_list *list)
{
    if (!list)
    {
        fprintf(stderr, "[free_list] 警告: 嘗試釋放空鏈表管理器, 跳過操作\n");
        return;
    }

    // 遍歷釋放每個節點
    node *current = list->head;
    while (current)
    {
        node *next_node = current->next; // 先保存下一個節點地址

        // 釋放節點內的動態數據
        free(current->data.title);
        free(current->data.artist);
        // 釋放節點本身
        free(current);

        current = next_node; // 移動到下一個節點
    }

    // 釋放鏈表管理器
    free(list);
    fprintf(stderr, "[free_list] 信息: 鏈表所有內存已釋放\n");
}

bool append(doubly_linked_list *list, song song_data)
{
    if (!list)
    {
        fprintf(stderr, "[append] 錯誤: 鏈表管理器爲空, 無法追加\n");
        return false;
    }

    // 創建新節點
    node *new_node = _create_node(song_data);
    if (!new_node)
    {
        fprintf(stderr, "[append] 錯誤: 無法追加歌曲「%s」\n", song_data.title);
        return false;
    }

    // 處理空鏈表/非空鏈表場景
    if (!list->head)
    {
        // 空鏈表: 新節點既是頭也是尾
        list->head = new_node;
        list->tail = new_node;
    }
    else
    {
        // 非空鏈表: 接在尾節點後
        list->tail->next = new_node;
        new_node->prev = list->tail;
        list->tail = new_node;
    }

    list->size++;

    fprintf(stderr, "[append] 信息: 歌曲「%s」已追加到尾部\n", song_data.title);

    return true;
}

bool prepend(doubly_linked_list *list, song song_data)
{
    if (!list)
    {
        fprintf(stderr, "[prepend] 錯誤: 鏈表管理器爲空, 無法插入\n");
        return false;
    }

    // 創建新節點
    node *new_node = _create_node(song_data);
    if (!new_node)
    {
        fprintf(stderr, "[prepend] 錯誤: 無法插入歌曲「%s」\n",
                song_data.title);
        return false;
    }

    // 處理空鏈表/非空鏈表場景
    if (list->head == NULL)
    {
        list->head = new_node;
        list->tail = new_node;
    }
    else
    {
        new_node->next = list->head;
        list->head->prev = new_node;
        list->head = new_node;
    }

    list->size++;

    fprintf(stderr, "[prepend] 信息: 歌曲「%s」已插入到頭部\n",
            song_data.title);

    return true;
}

bool insert_after(doubly_linked_list *list, node *target_node, song song_data)
{
    // 檢查參數有效
    if (!list || !target_node || !song_data.title || !song_data.artist)
    {
        fprintf(stderr,
                "[insert_after] 錯誤: 參數無效 (空鏈表/空節點/歌曲信息缺失)\n");
        return false;
    }

    // 創建新節點
    node *new_node = _create_node(song_data);
    if (!new_node)
    {
        fprintf(stderr, "[insert_after] 錯誤: 無法插入歌曲「%s」\n",
                song_data.title);
        return false;
    }

    // 調整指針關係
    new_node->next = target_node->next;
    new_node->prev = target_node;

    if (target_node->next)
    {
        // 目標節點不是尾節點: 更新原後續的prev
        target_node->next->prev = new_node;
    }
    else
    {
        // 目標節點是尾節點: 更新鏈表尾指針
        list->tail = new_node;
    }

    target_node->next = new_node;
    list->size++;

    fprintf(stderr, "[insert_after] 信息: 歌曲「%s」已插入到「%s」之後\n",
            song_data.title, target_node->data.title);

    return true;
}

bool delete_node(doubly_linked_list *list, node *node_to_delete)
{
    // 檢查參數有效性
    if (!list || !node_to_delete || list->size == 0)
    {
        fprintf(stderr, "[delete_node] 錯誤: 參數無效 (空鏈表/空節點)\n");
        return false;
    }

    // 保存刪除的歌曲標題(用於提示)
    char deleted_title[100];
    strncpy(deleted_title, node_to_delete->data.title,
            sizeof(deleted_title) - 1);
    deleted_title[sizeof(deleted_title) - 1] = '\0';

    // 調整指針關係(處理頭/尾/中間節點)
    if (node_to_delete->prev)
    {
        // 不是頭結點: 前續跳過當前節點
        node_to_delete->prev->next = node_to_delete->next;
    }
    else
    {
        // 是頭節點: 更新頭指針
        list->head = node_to_delete->next;
    }

    // 處理尾節點
    if (node_to_delete->next)
    {
        // 不是尾節點: 後續節點跳過當前節點
        node_to_delete->next->prev = node_to_delete->prev;
    }
    else
    {
        // 是尾節點: 更新尾指針爲前序節點
        list->tail = node_to_delete->prev;
    }

    // 釋放節點資源
    free(node_to_delete->data.title);
    free(node_to_delete->data.artist);
    free(node_to_delete);

    list->size--;

    fprintf(stderr, "[delete_node] 信息: 歌曲「%s」已刪除\n", deleted_title);

    return true;
}

node *find_by_title(const doubly_linked_list *list, const char *title)
{
    if (!list || !title || list->size == 0)
    {
        fprintf(stderr, "[find_by_title] 錯誤: 參數無效(空鏈表/空標題)\n");
        return NULL;
    }

    // 遍歷鏈表查詢
    node *current = list->head;
    while (current)
    {
        if (strcmp(current->data.title, title) == 0)
        {
            fprintf(stderr, "[find_by_title] 信息: 找到歌曲「%s」\n", title);
            return current;
        }
        current = current->next;
    }

    fprintf(stderr, "[find_by_title] 信息: 未找到歌曲「%s」\n", title);

    return NULL;
}

void print_list_forward(const doubly_linked_list *list)
{
    if (!list || list->size == 0)
    {
        printf("=== 鏈表爲空 ===\n");
        return;
    }

    printf("=== 正向打印鏈表 (共 %d 首歌) ===\n", list->size);

    node *current = list->head;
    int index = 1;
    while (current)
    {
        printf("%d. 標題: %s | 歌手: %s | 時長: %d秒\n", index++,
               current->data.title, current->data.artist,
               current->data.duration);
        current = current->next;
    }
    printf("==============================\n");
}

void print_list_backward(const doubly_linked_list *list)
{
    if (!list || list->size == 0)
    {
        printf("=== 鏈表爲空 ===\n");
        return;
    }

    printf("=== 反向打印鏈表 (共 %d 首歌) ===\n", list->size);

    node *current = list->tail;
    int index = list->size;
    while (current)
    {
        printf("%d. 標題: %s | 歌手: %s | 時長: %d秒\n", index--,
               current->data.title, current->data.artist,
               current->data.duration);
        current = current->prev;
    }
    printf("==============================\n");
}

