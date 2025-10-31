#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <stdbool.h>

// 歌曲數據結構(存儲單首歌曲信息)
typedef struct song
{
    char *title;  // 歌曲標題 (動態分配內存, 支持可變長度)
    char *artist; // 歌手名稱 (動態分配內存, 支持可變長度, 避免固定長度浪費)
    int duration; // 歌曲時長 (單位: 秒)
} song;

// 雙向鏈表節點結構 (每個節點關聯一首歌曲與前後節點)
typedef struct node
{
    song data;         // 節點存儲的歌曲數據
    struct node *next; // 指向「下一個節點」的指針
    struct node *prev; // 指向「前一個節點」的指針 (雙向核心)
} node;

// 雙向鏈表管理器 (統一管理鏈表狀態, 簡化外部操作)
typedef struct doubly_linked_list
{
    node *head; // 指向鏈表「頭結點」(第一首歌)
    node *tail; // 指向鏈表「尾節點」(最後一首歌)
    int size;   // 鏈表當前節點數 (快速獲取長度, 無需遍歷)
} doubly_linked_list;

// ----------------- 外部函數聲明 --------------------

/**
 * 創建空的雙向鏈表
 * @return 成功返回鏈表管理器指針, 失敗返回 NULL (內存分配失敗)
 */
doubly_linked_list *create_list(void);

/**
 * 釋放鏈表所有內存 (節點、歌曲動態字符串、管理器)
 * @param list 指向目標鏈表管理器
 */
void free_list(doubly_linked_list *list);

/**
 * 在鏈表末尾追加歌曲節點
 * @param list 指向目標節點
 * @param song_data 要追加的歌曲數據 (title/artist 不可爲 NULL)
 * @return 成功返回 true, 失敗返回 false (參數無效或內存分配失敗)
 */
bool append(doubly_linked_list *list, song song_data);

/**
 * 在鏈表頭部插入歌曲節點
 * @param list 指向目標鏈表
 * @param song_data 要插入的歌曲數據 (title/artist 不可爲 NULL)
 * @return 成功返回 true, 失敗返回 false (參數無效或內存分配失敗)
 */
bool prepend(doubly_linked_list *list, song song_data);

/**
 * 在指定節點後添加歌曲節點
 * @param list 指向目標鏈表
 * @param target_node 指向添加位置的目標節點(需屬於當前鏈表)
 * @param song_data 要插入的歌曲數據 (title/artisr 不可爲 NULL)
 * @return 成功返回 true, 失敗返回 false (參數無效或內存分配失敗)
 */
bool insert_after(doubly_linked_list *list, node *target_node, song song_data);

/**
 * 刪除鏈表中的指定節點
 * @param list 指向目標鏈表
 * @param node_to_delete 指向要刪除的節點(需屬於當前鏈表)
 * @return 成功返回 true, 失敗返回 false (參數無效或空鏈表)
 */
bool delete_node(doubly_linked_list *list, node *node_to_delete);

/**
 * 根據歌曲標題查詢節點 (區分大小寫, 精確匹配)
 * @param list 指向目標鏈表
 * @param title 要查詢的歌曲標題 (不可爲 NULL)
 * @return 找到返回節點指針, 未找到或參數無效返回 NULL
 */
node *find_by_title(const doubly_linked_list *list, const char *title);

/**
 * 正向打印鏈表 (從表頭到表尾)
 * @param list 指向目標鏈表 (空鏈表會打印提示)
 */
void print_list_forward(const doubly_linked_list *list);

/**
 * 反向打印鏈表 (從表尾到表頭, 體現雙向鏈表的優勢)
 * @param list 指向目標鏈表 (空鏈表會打印提示)
 */
void print_list_backward(const doubly_linked_list *list);

#endif // !DOUBLY_LINKED_LIST_H

