#pragma once

#include <stdbool.h>

/**
 * 不透明指针：隐藏Trie树内部实现细节
 */
typedef struct Trie Trie;

/**
 * 值销毁函数指针：用于释放存储在Trie中的用户自定义值
 *
 * @param value 待销毁的值的指针
 */
typedef void (*value_destroyer)(void *value);

/**
 * 创建Trie树实例
 *
 * @param destroyer 用于释放值的函数指针，简单类型可传NULL
 * @return 成功返回Trie指针，失败返回NULL
 */
Trie *trie_create(value_destroyer destroyer);

/**
 * 销毁Trie树，释放所有节点及存储的值
 *
 * @param trie 待销毁的Trie树指针
 */
void trie_destroy(Trie *trie);

/**
 * 向Trie树插入键值对
 *
 * @param trie Trie树指针
 * @param key 插入的键（小写字母字符串）
 * @param value 关联的值（非NULL，由Trie接管内存）
 * @return 插入成功返回true，失败返回false
 */
bool trie_insert(Trie *trie, const char *key, void *value);

/**
 * 查找键对应的 value
 *
 * @param trie Trie树指针
 * @param key 要查找的键
 * @return 找到返回值指针，否则返回NULL
 */
void *trie_search(const Trie *trie, const char *key);

/**
 * 检查Trie树中是否存在以指定前缀开头的键
 *
 * @param trie Trie树指针
 * @param prefix 前缀字符串
 * @return 存在返回true，否则返回false
 */
bool trie_starts_with(const Trie *trie, const char *prefix);

/**
 * 从Trie树中删除键及关联的值
 *
 * @param trie Trie树指针
 * @param key 要删除的键
 */
void trie_delete(Trie *trie, const char *key);

