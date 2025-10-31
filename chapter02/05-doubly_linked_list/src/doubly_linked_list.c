#include "doubly_linked_list.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h> // 線程安全支持 (C11標準)
#include <time.h>

/**
 * @brief 雙向鏈表節點結構 (內部實現, 用戶不可見)
 */
struct doubly_list_node
{
    void *data;                    // 數據指針 (指向用戶數據)
    struct doubly_list_node *next; // 後續節點指針
    struct doubly_list_node *prev; // 前驅節點指針
};

/**
 * @brief 內存池結構 (預分配節點, 減少動態內存分配開銷)
 * @deatils 管理固定大小的節點內存塊, 通過空閒鏈表快速分配/釋放節點
 */
typedef struct memory_pool
{
    doubly_list_node *pool_memory;    // 連續內存塊 (存儲所有預分配節點)
    doubly_list_node *free_list_head; // 空閒節點鏈表頭 (管理可分配節點)
    size_t total_nodes;               // 總節點數 (初始容量)
} memory_pool;

/**
 * @brief 雙向鏈表主結構
 */
struct doubly_linked_list
{
    doubly_list_node *head;   // 表頭結點
    doubly_list_node *tail;   // 表尾節點
    size_t size;              // 當前節點數量
    mtx_t lock;               // 互斥鎖 (確保多線程安全)
    memory_pool *pool;        // 節點內存池
    free_func user_free_func; // 用戶自定義數據釋放函數
};

/**
 * @brief 創建內存池 (內部函數)
 * @param initial_capacity 初始節點數量
 * @return 內存池指針, 失敗返回NULL
 */
static memory_pool *_memory_pool_create(size_t initial_capacity)
{
    // 處理無效容量 (默認創建10個節點)
    if (initial_capacity == 0)
    {
        fprintf(stderr,
                "[_memory_pool_create] 警告: 初始容量爲0, 使用默認值爲10.\n");
        initial_capacity = 10;
    }

    memory_pool *pool = (memory_pool *)malloc(sizeof(*pool));
    if (!pool)
    {
        perror("[_memory_pool_create] 內存池結構分配失敗");
        return NULL;
    }

    // 分配連續的節點內存塊
    pool->pool_memory =
        (doubly_list_node *)calloc(initial_capacity, sizeof(doubly_list_node));
    if (!pool->pool_memory)
    {
        perror("[_memory_pool_create] 節點內存塊分配失敗");
        free(pool);
        return NULL;
    }

    // 初始化空閒鏈表 (將所有節點串聯城鏈表)
    pool->free_list_head = &pool->pool_memory[0];
    pool->total_nodes = initial_capacity;

    for (size_t i = 0; i < initial_capacity - 1; ++i)
    {
        pool->pool_memory[i].next = &pool->pool_memory[i + 1];
        pool->pool_memory[i].prev = NULL; // 空閒節點前驅無效
        pool->pool_memory[i].data = NULL; // 初始化數據指針
    }

    // 最後一個空閒節點的後續爲NULL
    if (initial_capacity > 0)
    {
        pool->pool_memory[initial_capacity - 1].next = NULL;
        pool->pool_memory[initial_capacity - 1].prev = NULL;
        pool->pool_memory[initial_capacity - 1].data = NULL;
    }

    return pool;
}

/**
 * @brief 銷燬內存池 (內部函數)
 * @param pool 內存池指針
 */
static void _memory_pool_destroy(memory_pool *pool)
{
    if (!pool)
    {
        return;
    }
    free(pool->pool_memory); // 釋放連續節點內存塊
    free(pool);              // 釋放內存池結構
}

/**
 * @brief 從內存池分配節點 (內部函數)
 * @param pool 內存池指針
 * @return 可用節點指針, 空閒節點時返回NULL
 */
static doubly_list_node *_memory_pool_allocate_node(memory_pool *pool)
{
    if (!pool || !pool->free_list_head)
    {
        // 內存池消耗殆盡時可在此擴展(此處簡化處理爲返回NULL)
        fprintf(stderr,
                "[_memory_pool_allocate_node] 警告: 內存池節點已消耗殆盡\n");
        return NULL;
    }

    // 從空閒鏈表頭部獲取一個節點
    doubly_list_node *node = pool->free_list_head;
    pool->free_list_head = node->next; // 空閒鏈表頭後移

    // 重置節點狀態 (確保分配的節點乾淨)
    node->prev = NULL;
    node->next = NULL;
    node->data = NULL;

    return node;
}

/**
 * @brief 將節點釋放回內存池 (內部函數)
 * @param pool 內存池指針
 * @param node 要釋放的節點 (必須是從該內存池分配的)
 */
static void _memory_pool_free_node(memory_pool *pool, doubly_list_node *node)
{
    if (!pool || !node)
    {
        return;
    }

    // 檢查節點是否屬於當前內存池 (安全校驗)
    if (node < pool->pool_memory ||
        node >= pool->pool_memory + pool->total_nodes)
    {
        fprintf(stderr,
                "[_memory_pool_allocate_node] 錯誤: 節點不屬於當前內存池\n");
        return;
    }

    // 將節點插入空閒鏈表頭部
    node->next = pool->free_list_head;
    pool->free_list_head = node;
}

doubly_linked_list *list_create(size_t initial_capacity,
                                free_func free_function)
{
    doubly_linked_list *list = (doubly_linked_list *)malloc(sizeof(*list));
    if (!list)
    {
        perror("[list_create] 鏈表結構分配失敗");
        return NULL;
    }

    // 初始化內存池
    list->pool = _memory_pool_create(initial_capacity);
    if (!list->pool)
    {
        free(list);
        return NULL;
    }

    // 初始化鏈表狀態
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->user_free_func = free_function;

    // 初始化互斥鎖 (確保線程安全)
    if (mtx_init(&list->lock, mtx_plain) != thrd_success)
    {
        perror("[list_create] 互斥鎖初始化失敗");
        _memory_pool_destroy(list->pool);
        free(list);
        return NULL;
    }

    return list;
}

void list_destroy(doubly_linked_list **list_ptr)
{
    if (!list_ptr || !*list_ptr)
    {
        return;
    }

    doubly_linked_list *list = *list_ptr;
    mtx_lock(&list->lock); // 加鎖確保線程安全

    // 釋放所有節點的數據 (調用用戶自定義函數)
    doubly_list_node *current = list->head;
    while (current)
    {
        doubly_list_node *next_node = current->next;
        if (list->user_free_func && current->data)
        {
            list->user_free_func(current->data); // 釋放用戶數據
        }
        current = next_node;
    }

    // 釋放內存池和資源鎖
    _memory_pool_destroy(list->pool);
    mtx_unlock(&list->lock);
    mtx_destroy(&list->lock); // 銷燬互斥鎖

    // 釋放鏈表本身並置空指針
    free(list);
    *list_ptr = NULL;

    fprintf(stderr, "[list_destroy] 鏈表已完全銷燬\n");
}

/**
 * @brief 創建節點 (內部輔助函數)
 * @param list 鏈表指針
 * @param data 節點數據
 * @return 節點指針, 失敗返回NULL
 */
static doubly_list_node *_internal_create_node(doubly_linked_list *list,
                                               const void *data)
{
    if (!list || !list->pool)
    {
        return NULL;
    }

    doubly_list_node *new_node = _memory_pool_allocate_node(list->pool);
    if (!new_node)
    {
        return NULL;
    }

    new_node->data = (void *)data; // 存儲用戶數據指針 (注意: 此處爲淺拷貝)
    new_node->prev = NULL;
    new_node->next = NULL;

    return new_node;
}

bool list_append(doubly_linked_list *list, const void *data)
{
    if (!list || !data)
    {
        fprintf(stderr, "[list_append] 錯誤: 鏈表或數據爲空\n");
        return false;
    }

    mtx_lock(&list->lock); // 加鎖

    doubly_list_node *new_node = _internal_create_node(list, data);
    if (!new_node)
    {
        mtx_unlock(&list->lock);
        return false;
    }

    // 空鏈表: 新節點既是頭也是尾
    if (!list->tail)
    {
        list->head = new_node;
        list->tail = new_node;
    }
    else // 非空鏈表: 追加到尾部
    {
        list->tail->next = new_node;
        new_node->prev = list->tail;
        list->tail = new_node;
    }

    list->size++;

    mtx_unlock(&list->lock); // 解鎖

    return true;
}

bool list_prepend(doubly_linked_list *list, const void *data)
{
    if (!list || !data)
    {
        fprintf(stderr, "[list_prepend] 錯誤: 鏈表或數據爲空\n");
        return false;
    }

    mtx_lock(&list->lock);

    doubly_list_node *new_node = _internal_create_node(list, data);
    if (!new_node)
    {
        mtx_unlock(&list->lock);
        return false;
    }

    if (!list->head) // 空鏈表
    {
        list->head = new_node;
        list->tail = new_node;
    }
    else // 非空鏈表: 插入到頭部
    {
        new_node->next = list->head;
        list->head->prev = new_node;
        list->head = new_node;
    }

    list->size++;

    mtx_unlock(&list->lock);

    return true;
}

bool list_insert_after(doubly_linked_list *list, doubly_list_node *node,
                       const void *data)
{
    if (!list || !data)
    {
        fprintf(stderr, "[list_insert_after] 錯誤: 參數爲空\n");
        return false;
    }

    mtx_lock(&list->lock);

    doubly_list_node *new_node = _internal_create_node(list, data);
    if (!new_node)
    {
        mtx_unlock(&list->lock);
        return false;
    }

    // 調整指針關係
    new_node->next = node->next;
    new_node->prev = node;

    if (node->next) // 目標節點不是尾節點
    {
        node->next->prev = new_node;
    }
    else // 目標節點是尾節點: 更新鏈表尾
    {
        list->tail = new_node;
    }

    node->next = new_node;

    list->size++;

    mtx_unlock(&list->lock);

    return true;
}

void list_delete_node(doubly_linked_list *list, doubly_list_node *node)
{
    if (!list || !node)
    {
        return;
    }

    mtx_lock(&list->lock);

    // 調整前驅節點指針
    if (node->prev)
    {
        node->prev->next = node->next;
    }
    else // 刪除尾節點
    {
        list->tail = node->prev;
    }

    // 釋放節點數據 (用戶自定義邏輯)
    if (list->user_free_func && node->data)
    {
        list->user_free_func(node->data);
    }

    // 節點放回內存池 (而非直接free)
    _memory_pool_free_node(list->pool, node);
    list->size--;

    mtx_unlock(&list->lock);
}

doubly_list_node *list_find(doubly_linked_list *list, const void *data_to_find,
                            compare_func compare_function)
{
    if (!list || !data_to_find || !compare_function)
    {
        fprintf(stderr, "[list_find] 錯誤: 參數爲空\n");
        return NULL;
    }

    mtx_lock(&list->lock);

    doubly_list_node *current = list->head;
    while (current)
    {
        if (compare_function(current->data, data_to_find) == 0)
        {
            mtx_unlock(&list->lock); // 找到後解鎖
            return current;
        }
        current = current->next;
    }

    mtx_unlock(&list->lock);

    return NULL; // 爲找到
}

size_t list_get_size(const doubly_linked_list *list)
{
    return (list) ? list->size : 0;
}

void list_for_each(doubly_linked_list *list, action_func action_function,
                   void *context)
{
    if (!list || !action_function)
    {
        return;
    }

    mtx_lock(&list->lock); // 加鎖確保遍歷過程中數據不被修改

    doubly_list_node *current = list->head;
    while (current)
    {
        action_function(current->data, context); // 執行用戶操作
        current = current->next;
    }

    mtx_unlock(&list->lock);
}

void *list_get_data(const doubly_list_node *node)
{
    return (node) ? node->data : NULL;
}

