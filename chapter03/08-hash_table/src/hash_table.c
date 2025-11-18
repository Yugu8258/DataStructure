// hash_table.c
#define _CRT_SECURE_NO_WARNINGS // 允许使用 strcpy 等函数
#include "hash_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Private Structure Definitions ---

// 哈希表中的节点，用于构成每个桶中的链表
typedef struct Node
{
    char *key;         // 指向动态分配的键字符串
    char *value;       // 指向动态分配的值字符串
    struct Node *next; // 指向链表中的下一个节点
} Node;

// 哈希表的实际管理结构
struct HashTable
{
    Node **buckets;  // 指向一个指针数组，每个指针是一个桶（链表的头节点）
    size_t capacity; // 桶的数量
    size_t size;     // 当前存储的键值对总数
};

// --- Static Helper Functions (Private) ---

// 哈希函数：将字符串转换为一个在 [0, capacity-1] 范围内的索引
// 这与您的HTML示例中的哈希函数逻辑相同。
// 一个简单的字符求和哈希函数，适用于教学和小规模数据。
// 注意：实际应用中应使用更复杂的哈希函数以减少冲突。
static size_t hash_function(const char *key, size_t capacity)
{
    unsigned long hash_value = 0;
    for (size_t i = 0; key[i] != '\0'; i++)
    {
        hash_value += key[i];
    }
    return hash_value % capacity;
}

// --- Public API Implementations ---

HashTable *ht_create(size_t capacity)
{
    if (capacity == 0)
        return NULL;
    HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
    if (!ht)
        return NULL;

    // 为桶数组分配内存，这是一个指针数组，所以是 sizeof(Node*)
    ht->buckets = (Node **)calloc(capacity, sizeof(Node *));
    if (!ht->buckets)
    {
        free(ht);
        return NULL;
    }
    // `calloc` 会将所有桶的头指针初始化为 NULL，这是必需的。

    ht->capacity = capacity;
    ht->size = 0;
    return ht;
}

void ht_destroy(HashTable **p_ht)
{
    if (!p_ht || !*p_ht)
        return;
    HashTable *ht = *p_ht;

    // 遍历每一个桶
    for (size_t i = 0; i < ht->capacity; i++)
    {
        // 遍历桶中的链表，释放每一个节点
        Node *current = ht->buckets[i];

        // 增加防御性检查，确保 to_free->key 和 to_free->value 非 NULL 后再释放
        while (current != NULL)
        {
            Node *to_free = current;
            current = current->next;
            if (to_free->key)
            {
                free(to_free->key); // 释放键的拷贝
            }
            if (to_free->value)
            {
                free(to_free->value); // 释放值的拷贝
            }
            free(to_free); // 释放节点本身
        }
    }
    free(ht->buckets); // 释放桶数组
    free(ht);          // 释放哈希表结构体
    *p_ht = NULL;      // 防止悬空指针
}

bool ht_set(HashTable *ht, const char *key, const char *value)
{
    /*
     * 函数功能: [工业级] 插入或更新一个键值对。
     * ------------------------- 教学案例 -------------------------
     * 假设 capacity = 10, hash("name") = 4, hash("mane") = 4 (哈希冲突)
     *
     * 案例 1: 插入 ("name", "Alice")
     *   - 计算哈希得到索引 4。
     *   - 桶 4 为空，直接创建一个新节点并插入。
     *   - 桶 4: -> [("name", "Alice")] -> NULL
     *
     * 案例 2: 插入 ("mane", "Bob")
     *   - 计算哈希得到索引 4。
     *   - 桶 4 不为空，遍历链表，未找到键 "mane"。
     *   - 创建新节点，并将其插入到链表头部 (最简单的方式)。
     *   - 桶 4: -> [("mane", "Bob")] -> [("name", "Alice")] -> NULL
     *
     * 案例 3: 更新 ("name", "Amy")
     *   - 计算哈希得到索引 4。
     *   - 遍历桶 4 的链表，使用 strcmp 找到键 "name"。
     *   - 释放旧值 "Alice" 的内存，分配新内存并存入 "Amy"。
     *   - 桶 4: -> [("mane", "Bob")] -> [("name", "Amy")] -> NULL
     * -----------------------------------------------------------
     */

    // 步骤 1: [防御性编程] 检查所有输入参数是否有效。
    if (!ht || !key || !value)
        return false;

    // 步骤 2: 计算键的哈希值，确定它属于哪个桶。
    size_t index = hash_function(key, ht->capacity);

    // 步骤 3: 遍历桶中的链表，检查键是否已经存在。
    Node *current = ht->buckets[index];
    while (current != NULL)
    {
        if (strcmp(current->key, key) == 0)
        {
            // --- 情况A: 键已存在，执行“更新”操作 ---
            // 为了防止内存泄漏，先释放旧值的内存。
            free(current->value);
            // 为新值分配内存并拷贝。
            current->value = (char *)malloc(strlen(value) + 1);
            if (!current->value)
                return false; // 内存分配失败
            strcpy(current->value, value);
            return true; // 更新成功，函数结束
        }
        current = current->next;
    }

    // --- 情况B: 键不存在 (循环走完都未找到)，执行“插入”操作 ---
    // 步骤 4: 创建一个新的节点。
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node)
        return false;

    // 为键和值分配独立的内存并拷贝，这样哈希表就不依赖于外部传入的指针生命周期。
    new_node->key = (char *)malloc(strlen(key) + 1);
    new_node->value = (char *)malloc(strlen(value) + 1);

    // 健壮性检查：如果任何一个malloc失败，都需要清理已分配的内存。
    if (!new_node->key || !new_node->value)
    {
        free(new_node->key); // free(NULL)是安全的
        free(new_node->value);
        free(new_node);
        return false;
    }
    strcpy(new_node->key, key);
    strcpy(new_node->value, value);

    // 步骤 5: 将新节点插入到链表的头部。
    // 这是最简单高效的链表插入方式。
    new_node->next = ht->buckets[index]; // 新节点的 next 指向当前桶的第一个节点
    ht->buckets[index] = new_node;       // 更新桶的头指针，使其指向这个新节点

    // 步骤 6: 更新哈希表的总大小。
    ht->size++;
    return true; // 插入成功
}

const char *ht_get(const HashTable *ht, const char *key)
{
    /*
     * 函数功能: [工业级] 根据键查找对应的值。
     * 教学案例: 在 `ht_set` 案例 3 的状态下查找 "name"。
     *   - 桶 4: -> [("mane", "Bob")] -> [("name", "Amy")] -> NULL
     *   - 计算哈希("name")得到索引 4。
     *   - 遍历桶 4 的链表。
     *   - 第一个节点 "mane" != "name"。
     *   - 第二个节点 "name" == "name"，找到！
     *   - 返回指向内部值 "Amy" 的指针。
     */
    if (!ht || !key)
        return NULL;

    size_t index = hash_function(key, ht->capacity);

    Node *current = ht->buckets[index];
    while (current != NULL)
    {
        if (strcmp(current->key, key) == 0)
        {
            return current->value; // 找到，返回指向内部值的指针
        }
        current = current->next;
    }

    return NULL; // 遍历完链表仍未找到
}

bool ht_remove(HashTable *ht, const char *key)
{
    /*
     * 函数功能: [工业级] 根据键删除一个键值对。
     * 教学案例: 在 `ht_set` 案例 3 的状态下删除 "mane"。
     *   - 桶 4: -> [("mane", "Bob")] -> [("name", "Amy")] -> NULL
     *   - 计算哈希("mane")得到索引 4。
     *   - 遍历链表，需要一个 `prev` 指针来辅助删除。
     *   - 找到 "mane" 时，它是头节点 (prev=NULL)。
     *   - 将桶的头指针 `buckets[4]` 指向 "mane" 的下一个节点 ("name")。
     *   - 释放 "mane" 节点的键、值和节点本身。
     *   - 最终桶 4: -> [("name", "Amy")] -> NULL
     *
     * [桶的入口] -> Alice -> Bob -> Charile -> David -> NULL
     * ht->buckets[index] = Alice
     * current = Alice
     *
     */
    if (!ht || !key)
        return false;

    size_t index = hash_function(key, ht->capacity);

    Node *current = ht->buckets[index];
    Node *prev = NULL; // 关键: 用于链接前后节点的 "前驱" 指针

    // 遍历链表查找要删除的节点
    while (current != NULL && strcmp(current->key, key) != 0)
    {
        prev = current;
        current = current->next;
    }

    // 如果 current 为 NULL，说明遍历完也没找到这个键
    if (current == NULL)
    {
        return false;
    }

    // --- 找到了要删除的节点 (current) ---
    if (prev == NULL)
    {
        // 情况A: 要删除的是链表的头节点
        ht->buckets[index] = current->next;
    }
    else
    {
        // 情况B: 要删除的是链表的中间或尾部节点
        prev->next = current->next;
    }

    // 释放被删除节点的所有内存
    free(current->key);
    free(current->value);
    free(current);

    ht->size--;
    return true; // 删除成功
}

void ht_print(const HashTable *ht)
{
    if (!ht)
    {
        printf("HashTable is NULL.\n");
        return;
    }
    printf("--- HashTable (capacity: %zu, size: %zu) ---\n", ht->capacity,
           ht->size);
    for (size_t i = 0; i < ht->capacity; i++)
    {
        printf("Bucket[%zu]: ", i);
        Node *current = ht->buckets[i];
        if (current == NULL)
        {
            printf("-> NULL\n");
        }
        else
        {
            while (current != NULL)
            {
                printf("-> [K:\"%s\", V:\"%s\"] ", current->key,
                       current->value);
                current = current->next;
            }
            printf("-> NULL\n");
        }
    }
    printf("------------------------------------------\n\n");
}

