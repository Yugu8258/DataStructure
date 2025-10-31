#include "circular_doubly_linked_list.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>

/**
 * @brief 雙向循環鏈表節點結構
 */
struct circular_doubly_list_node
{
    void *data;                             // 數據指針
    struct circular_doubly_list_node *next; // 後續節點 (循環指向)
    struct circular_doubly_list_node *prev; // 前驅節點 (循環指向)
};

/**
 * @brief 內存池結構 (管理節點內存分配)
 */
typedef struct memory_pool
{
    circular_doubly_list_node *pool_memory;    // 連續節點內存塊
    circular_doubly_list_node *free_list_head; // 空閒節點鏈表頭
    size_t total_nodes;                        // 總節點數 (初始容量)
} memory_pool;

/**
 * @brief 循環雙向鏈表主結構
 */
struct circular_doubly_linked_list
{
    circular_doubly_list_node
        *head;                // 頭結點 (任意節點均可作爲頭, 通過旋轉切換)
    size_t size;              // 當前節點數量
    mtx_t lock;               // 互斥鎖 (線程安全)
    memory_pool *pool;        // 節點內存池
    free_func user_free_func; // 數據釋放回調函數
};

/**
 * @brief 創建內存池 (內部函數)
 * @param initial_capacity 初始節點數量
 * @return 內存池指針, 失敗返回NULL
 */
static memory_pool *_memory_pool_create(size_t initial_capacity)
{
    if (initial_capacity == 0)
    {
        fprintf(stderr, "[_memory_pool_create] 錯誤: 初始容量不能爲0\n");
        return NULL;
    }

    memory_pool *pool = (memory_pool *)malloc(sizeof(*pool));
    if (!pool)
    {
        perror("[_memory_pool_create] 內存池結構分配失敗");
        return NULL;
    }

    // 分配連續節點內存 (用calloc初始化避免野指針)
    pool->pool_memory = (circular_doubly_list_node *)calloc(
        initial_capacity, sizeof(circular_doubly_list_node));
    if (!pool->pool_memory)
    {
        perror("[_memory_pool_create] 節點內存塊分配失敗");
        free(pool);
        return NULL;
    }

    // 初始化空閒鏈表 (串聯所有節點)
    pool->free_list_head = &pool->pool_memory[0];
    pool->total_nodes = initial_capacity;

    for (size_t i = 0; i < initial_capacity - 1; ++i)
    {
        pool->pool_memory[i].next = &pool->pool_memory[i + 1];
    }
    pool->pool_memory[initial_capacity - 1].next =
        NULL; // 最後一個節點後續爲NULL

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

    free(pool->pool_memory); // 釋放節點內存塊
    free(pool);              // 釋放內存池結構
}

/**
 * @brief 從內存池分配節點 (內部函數)
 * @param pool 內存池指針
 * @return 可用節點指針, 無空閒節點返回NULL
 */
static circular_doubly_list_node *_memory_pool_allocate_node(memory_pool *pool)
{
    if (!pool || !pool->free_list_head)
    {
        fprintf(stderr,
                "[_memory_pool_allocate_node] 警告: 內存池節點已耗盡\n");
        return NULL;
    }

    // 從空閒鏈表頭部獲取節點
    circular_doubly_list_node *node = pool->free_list_head;
    pool->free_list_head = node->next;

    // 重置節點狀態
    node->prev = NULL;
    node->next = NULL;
    node->data = NULL;

    return node;
}

/**
 * @brief 將節點釋放回內存池 (內部函數)
 * @param pool 內存池指針
 * @param node 待釋放節點 (必須屬於當前內存池)
 */
static void _memory_pool_free_node(memory_pool *pool,
                                   circular_doubly_list_node *node)
{
    if (!pool || !node)
    {
        return;
    }

    // 校驗節點是否屬於當前內存池 (安全檢查)
    if (node < pool->pool_memory ||
        node >= pool->pool_memory + pool->total_nodes)
    {
        fprintf(stderr,
                "[_memory_pool_free_node] 錯誤: 節點不屬於當前內存池\n");
        return;
    }

    // 將節點插入空閒鏈表頭部
    node->next = pool->free_list_head;
    pool->free_list_head = node;
}

circular_doubly_linked_list *list_create(size_t initial_capacity,
                                         free_func free_function)
{
    circular_doubly_linked_list *list =
        (circular_doubly_linked_list *)malloc(sizeof(*list));
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
    list->size = 0;
    list->user_free_func = free_function;

    // 初始化互斥鎖 (線程安全)
    if (mtx_init(&list->lock, mtx_plain) != thrd_success)
    {
        perror("[list_create] 互斥鎖初始化失敗");
        _memory_pool_destroy(list->pool);
        free(list);
        return NULL;
    }

    return list;
}

void list_destroy(circular_doubly_linked_list **list_ptr)
{
    if (!list_ptr || !*list_ptr)
    {
        return;
    }

    circular_doubly_linked_list *list = *list_ptr;
    mtx_lock(&list->lock); // 加鎖確保線程安全

    // 釋放所有節點數據 (調用用戶自定義函數)
    if (list->head != NULL && list->size > 0)
    {
        circular_doubly_list_node *current = list->head;
        for (size_t i = 0; i < list->size; ++i)
        {
            circular_doubly_list_node *next_node = current->next;
            if (list->user_free_func && current->data)
            {
                list->user_free_func(current->data);
            }
            current = next_node;
        }
    }

    // 釋放內存池和鎖資源
    _memory_pool_destroy(list->pool);
    mtx_unlock(&list->lock);
    mtx_destroy(&list->lock);

    // 釋放鏈表本身並置空指針
    free(list);
    *list_ptr = NULL;

    fprintf(stderr, "[list_destroy] 鏈表已完全銷燬\n");
}

bool list_append(circular_doubly_linked_list *list, void *data)
{
    if (!list || !data)
    {
        fprintf(stderr, "[list_append] 錯誤: 鏈表或數據爲空\n");
        return false;
    }

    mtx_lock(&list->lock);

    circular_doubly_list_node *new_node =
        _memory_pool_allocate_node(list->pool);
    if (!new_node)
    {
        mtx_unlock(&list->lock);
        return false;
    }

    new_node->data = data;

    if (list->head == NULL) // 鏈表爲空: 新節點自循環
    {
        list->head = new_node;
        new_node->next = new_node;
        new_node->prev = new_node;
    }
    else // 非空鏈表: 插入到尾節點後 (頭結點的前驅是尾節點)
    {
        circular_doubly_list_node *tail = list->head->prev;
        // 建立新節點與尾節點的關係
        tail->next = new_node;
        new_node->prev = tail;

        // 建立新節點與頭結點的關係 (維持循環)
        new_node->next = list->head;
        list->head->prev = new_node;
    }

    list->size++;

    mtx_unlock(&list->lock);

    return true;
}

bool list_prepend(circular_doubly_linked_list *list, void *data)
{
    if (!list || !data)
    {
        fprintf(stderr, "[list_prepend] 錯誤: 鏈表或數據爲空\n");
        return false;
    }

    // 先追加到尾部, 再通過旋轉將其設爲頭結點
    if (!list_append(list, data))
    {
        return false;
    }

    // 若鏈表只有一節節點, 無需旋轉(本身就是頭節點)
    if (list->size > 1)
    {
        mtx_lock(&list->lock);
        list->head = list->head->prev; // 新節點是原尾節點, 設爲頭
        mtx_unlock(&list->lock);
    }

    return true;
}

void list_delete_node(circular_doubly_linked_list *list,
                      circular_doubly_list_node *node)
{
    if (!list || !node || list->size == 0)
    {
        return;
    }

    mtx_lock(&list->lock);

    // 特殊情況: 鏈表只有一個節點
    if (list->size == 1)
    {
        list->head = NULL; // 清空鏈表
    }
    else
    {
        // 調整前驅和後續節點的指針 (維持循環)
        node->prev->next = node->next;
        node->next->prev = node->prev;

        // 若刪除的是頭結點, 更新頭結點爲下一個節點
        if (node == list->head)
        {
            list->head = node->next;
        }
    }

    // 釋放節點數據和節點本身
    if (list->user_free_func && node->data)
    {
        list->user_free_func(node->data);
    }

    _memory_pool_free_node(list->pool, node);

    list->size--;

    mtx_unlock(&list->lock);
}

circular_doubly_list_node *list_find_node(circular_doubly_linked_list *list,
                                          const void *data_to_find,
                                          compare_func compare_function)
{
    if (!list || !data_to_find || !compare_function || list->size == 0)
    {
        return NULL;
    }

    mtx_lock(&list->lock);

    circular_doubly_list_node *current = list->head;
    circular_doubly_list_node *found_node = NULL;

    // 遍歷所有節點 (循環鏈表需限制次數, 避免死循環)
    for (size_t i = 0; i < list->size; ++i)
    {
        if (compare_function(current->data, data_to_find) == 0)
        {
            found_node = current;
            break;
        }
        current = current->next;
    }

    mtx_unlock(&list->lock);

    return found_node;
}

void list_rotate_forward(circular_doubly_linked_list *list)
{
    // 節點數補足無需旋轉
    if (!list || list->size < 2)
    {
        return;
    }

    mtx_lock(&list->lock);

    list->head = list->head->next; // 頭結點變爲源頭結點的後續
    mtx_unlock(&list->lock);
}

void list_rotate_backward(circular_doubly_linked_list *list)
{
    // 節點數補足無需旋轉
    if (!list || list->size < 2)
    {
        return;
    }

    mtx_lock(&list->lock);

    list->head = list->head->prev; // 頭結點變爲源頭結點的前驅
    mtx_unlock(&list->lock);
}

size_t list_get_size(const circular_doubly_linked_list *list)
{
    return (list) ? list->size : 0;
}

void list_for_each(circular_doubly_linked_list *list,
                   action_func action_function, void *context)
{
    if (!list || !action_function || list->size == 0 || !list->head)
    {
        return;
    }

    mtx_lock(&list->lock);

    circular_doubly_list_node *current = list->head;

    for (size_t i = 0; i < list->size; ++i)
    {
        action_function(current->data, context); // 執行用戶操作
        current = current->next;
    }

    mtx_unlock(&list->lock);
}

void *circular_doubly_list_node_get_data(const circular_doubly_list_node *node)
{
    return (node) ? node->data : NULL;
}

circular_doubly_list_node *list_get_head_node(circular_doubly_linked_list *list)
{
    if (!list)
    {
        return NULL;
    }

    mtx_lock(&list->lock);

    circular_doubly_list_node *head = list->head;

    mtx_unlock(&list->lock);

    return head;
}

