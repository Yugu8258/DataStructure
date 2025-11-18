#include "trie.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

// 英文字母表大小（仅支持小写字母a-z）
#define ALPHABET_SIZE 26

/**
 * Trie节点结构
 *
 * - value: 存储键对应的值（非NULL表示该节点是一个键的结尾）
 * - children: 子节点数组（索引0-25对应字母a-z）
 */
typedef struct TrieNode
{
    void *value;                              // 键值对的值
    struct TrieNode *children[ALPHABET_SIZE]; // 子节点指针数组
} TrieNode;

/**
 * Trie树结构
 *
 * - root: 根节点（不存储实际字符）
 * - value_destroyer: 用户提供的value销毁函数
 */
struct Trie
{
    TrieNode *root;
    value_destroyer valueDestroyer;
};

// 私有函数声明
static TrieNode *_trie_node_create(void);
static void _trie_node_destroy_recursive(TrieNode *node,
                                         value_destroyer destroyer);
static int _char_to_index(char c);
static bool _is_node_empty(TrieNode *node);
static bool _trie_node_delete_recursive_helper(TrieNode **pNode,
                                               const char *key, int depth,
                                               value_destroyer destroyer);

/**
 * 创建新的Trie节点
 *
 * @return 成功返回节点指针，失败返回NULL
 */
static TrieNode *_trie_node_create(void)
{
    TrieNode *node = (TrieNode *)malloc(sizeof(TrieNode));
    if (node)
    {
        node->value = NULL; // 初始值为NULL（非键结尾）
        // 初始化子节点数组为NULL
        for (int i = 0; i < ALPHABET_SIZE; ++i)
        {
            node->children[i] = NULL;
        }
    }
    return node;
}

/**
 * 递归销毁Trie节点及其子树
 *
 * @param node 待销毁的节点
 * @param destroyer 用于释放value的函数
 */
static void _trie_node_destroy_recursive(TrieNode *node,
                                         value_destroyer destroyer)
{
    if (!node)
    {
        return;
    }

    // 先递归销毁所有子节点（深度优先）
    for (int i = 0; i < ALPHABET_SIZE; ++i)
    {
        _trie_node_destroy_recursive(node->children[i], destroyer);
    }

    // 释放当前节点存储的value
    if (node->value && destroyer)
    {
        destroyer(node->value);
    }

    // 释放节点本身
    free(node);
}

/**
 * 将字符转换为索引（a-z -> 0-25）
 *
 * @param c 输入字符（小写字母）
 * @return 成功返回0-25，失败返回-1
 */
static int _char_to_index(char c)
{
    if (c >= 'a' && c <= 'z')
    {
        return c - 'a'; // 映射a->0, b->1, ..., z->25
    }
    return -1; // 无效字符
}

/**
 * 检查节点是否为空（无value且无子女）
 *
 * @param node 待检查节点
 * @return 空返回true，否则返回false
 */
static bool _is_node_empty(TrieNode *node)
{
    if (!node)
    {
        return true;
    }

    // 有value则不为空
    if (node->value != NULL)
    {
        return false;
    }
    // 有子女则不为空
    for (int i = 0; i < ALPHABET_SIZE; ++i)
    {
        if (node->children[i] != NULL)
        {
            return false;
        }
    }
    return true;
}

/**
 * 递归删除键的辅助函数
 *
 * @param pNode 节点指针的指针（用于修改父节点的指针）
 * @param key 要删除的键
 * @param depth 当前递归深度（对应键的字符索引）
 * @param destroyer value销毁函数
 * @return 节点为空可删除返回true，否则返回false
 */
static bool _trie_node_delete_recursive_helper(TrieNode **pNode,
                                               const char *key, int depth,
                                               value_destroyer destroyer)
{
    if (!pNode || !*pNode)
    {
        return false; // 节点不存在，无需删除
    }

    TrieNode *node = *pNode;

    // 递归终止条件：到达键的结尾
    if (key[depth] == '\0')
    {
        // 释放当前节点的value
        if (node->value)
        {
            if (destroyer)
            {
                destroyer(node->value);
            }
            node->value = NULL; // 标记为非键结尾
        }

        // 若节点为空，则删除该节点
        if (_is_node_empty(node))
        {
            free(node);
            *pNode = NULL; // 父节点不再指向该节点
            return true;
        }
        return false;
    }

    // 递归处理下一个字符
    int index = _char_to_index(key[depth]);
    if (index == -1)
    {
        return false; // 无效字符，删除失败
    }

    // 递归删除子节点
    if (_trie_node_delete_recursive_helper(&node->children[index], key,
                                          depth + 1, destroyer))
    {
        // 子节点被删除后，检查当前节点是否为空
        if (_is_node_empty(node))
        {
            free(node);
            *pNode = NULL;
            return true;
        }
    }

    return false;
}

// 公共API实现

Trie *trie_create(value_destroyer destroyer)
{
    Trie *trie = (Trie *)malloc(sizeof(Trie));
    if (!trie)
    {
        return NULL; // 内存分配失败
    }

    // 创建根节点
    trie->root = _trie_node_create();
    if (!trie->root)
    {
        free(trie);
        return NULL;
    }

    trie->valueDestroyer = destroyer;
    return trie;
}

void trie_destroy(Trie *trie)
{
    if (!trie)
        return;

    // 递归销毁所有节点
    _trie_node_destroy_recursive(trie->root, trie->valueDestroyer);
    free(trie);
}

bool trie_insert(Trie *trie, const char *key, void *value)
{
    // 参数校验
    if (!trie || !trie->root || !key || !value)
    {
        return false;
    }

    TrieNode *crawl = trie->root; // 遍历指针
    int len = strlen(key);

    // 遍历键的每个字符
    for (int i = 0; i < len; ++i)
    {
        int index = _char_to_index(key[i]);
        if (index == -1)
        {
            return false; // 包含无效字符，插入失败
        }

        // 若子节点不存在则创建
        if (!crawl->children[index])
        {
            crawl->children[index] = _trie_node_create();
            if (!crawl->children[index])
            {
                return false; // 节点创建失败
            }
        }

        // 移动到下一个节点
        crawl = crawl->children[index];
    }

    // 覆盖旧值（注意：此处不自动释放旧值，需用户手动处理）
    crawl->value = value;
    return true;
}

void *trie_search(const Trie *trie, const char *key)
{
    // 参数校验
    if (!trie || !trie->root || !key)
    {
        return NULL;
    }

    const TrieNode *crawl = trie->root;
    int len = strlen(key);

    // 遍历键的每个字符
    for (int i = 0; i < len; ++i)
    {
        int index = _char_to_index(key[i]);
        if (index == -1 || !crawl->children[index])
        {
            return NULL; // 字符无效或路径不存在
        }
        crawl = crawl->children[index];
    }

    // 返回值（NULL表示不是键的结尾）
    return crawl->value;
}

bool trie_starts_with(const Trie *trie, const char *prefix)
{
    // 参数校验
    if (!trie || !trie->root || !prefix)
    {
        return false;
    }

    const TrieNode *crawl = trie->root;
    int len = strlen(prefix);

    // 遍历前缀的每个字符
    for (int i = 0; i < len; ++i)
    {
        int index = _char_to_index(prefix[i]);
        if (index == -1 || !crawl->children[index])
        {
            return false; // 前缀不存在
        }
        crawl = crawl->children[index];
    }

    return true; // 前缀存在
}

void trie_delete(Trie *trie, const char *key)
{
    // 参数校验
    if (!trie || !trie->root || !key || *key == '\0')
    {
        return;
    }

    // 递归删除键
    _trie_node_delete_recursive_helper(&trie->root, key, 0, trie->valueDestroyer);
}

