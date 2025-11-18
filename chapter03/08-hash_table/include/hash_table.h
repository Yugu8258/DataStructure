#pragma once
#include <stdbool.h>
#include <stddef.h>

// --- Opaque Pointer Declaration ---
// 用户只知道有 HashTable 这个类型，但无法访问其内部成员，实现了完美的封装。
typedef struct HashTable HashTable;

// --- Public API Prototypes ---

/**
 * @brief 创建一个新的哈希表。
 *
 * @param capacity
 * 哈希表的容量（桶的数量）。选择一个合适的素数通常能获得更好的分布。
 * @return 成功时返回指向新哈希表的指针，如果内存分配失败或容量为0则返回 NULL。
 */
HashTable *ht_create(size_t capacity);

/**
 * @brief 销毁一个哈希表，并释放所有相关内存（包括所有键和值）。
 *
 * @param p_ht 指向哈希表指针的指针。函数执行后，*p_ht 将被设置为 NULL。
 */
void ht_destroy(HashTable **p_ht);

/**
 * @brief 在哈希表中插入或更新一个键值对。
 *
 * 如果键已存在，其对应的值将被更新。
 * 如果键不存在，将创建一个新的键值对并插入。
 * 函数会为键和值创建独立的内存拷贝，所以调用者可以释放自己的版本。
 *
 * @param ht 指向要操作的哈希表。
 * @param key 要插入/更新的键（一个C字符串）。
 * @param value 关联的值（一个C字符串）。
 * @return 成功返回 true，如果参数无效或内存分配失败则返回 false。
 */
bool ht_set(HashTable *ht, const char *key, const char *value);

/**
 * @brief 从哈希表中查找一个键并返回其对应的值。
 *
 * @param ht 指向要操作的哈希表。
 * @param key 要查找的键。
 * @return 如果找到，返回一个指向值的常量字符串指针。
 *         **警告**: 返回的指针指向哈希表内部数据，不应被修改或释放。
 *         如果未找到，返回 NULL。
 */
const char *ht_get(const HashTable *ht, const char *key);

/**
 * @brief 从哈希表中删除一个键值对。
 *
 * @param ht 指向要操作的哈希表。
 * @param key 要删除的键。
 * @return 如果找到并成功删除则返回 true，否则返回 false。
 */
bool ht_remove(HashTable *ht, const char *key);

/**
 * @brief (辅助函数) 打印哈希表的内部结构，用于调试和教学。
 */
void ht_print(const HashTable *ht);

