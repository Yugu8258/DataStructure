// bst.h binary_search_tree

#include "bst.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Private Structure Definitions ---

/**
 * 节点结构体：BST的基本组成单元
 */
typedef struct Node
{
    void *data;         // 存储元素数据（泛型指针）
    struct Node *left;  // 左子节点（小于当前节点）
    struct Node *right; // 右子节点（大于当前节点）
} Node;

/**
 * BST结构体：管理树的元信息
 */
struct BST
{
    Node *root;          // 根节点
    size_t element_size; // 元素大小（字节）
    size_t size;         // 节点数量
    CompareFunc compare; // 比较函数
};

// --- Static Helper Functions (Recursive Implementations) ---

/**
 * 创建新节点
 *
 * @param data 元素数据
 * @param element_size 元素大小
 * @return 成功返回节点指针，失败返回NULL
 */
static Node *_create_node(const void *data, size_t element_size)
{
    if (!data || element_size == 0)
    {
        fprintf(stderr, "[error: _create_node] invalid arguments\n");
        return NULL;
    }

    Node *node = (Node *)malloc(sizeof(Node));
    if (!node)
    {
        fprintf(stderr, "[error: _create_node] failed to allocate memory\n");
        return NULL;
    }

    // 深拷贝数据（避免依赖外部内存）
    node->data = malloc(element_size);
    if (!node->data)
    {
        fprintf(stderr, "[error: _create_node] failed to allocate memory\n");
        free(node);
        return NULL;
    }
    memcpy(node->data, data, element_size);

    node->left = node->right = NULL;
    return node;
}

/**
 * 递归销毁子树（后序遍历）
 *
 * @param node 子树根节点
 */
static void _destroy_subtree(Node *node)
{
    if (!node)
    {
        return;
    }
    _destroy_subtree(node->left);  // 先销毁左子树
    _destroy_subtree(node->right); // 再销毁右子树
    free(node->data);              // 释放数据
    free(node);                    // 释放节点
}

/**
 * 递归插入节点
 *
 * @param bst BST指针
 * @param node 当前节点
 * @param data 待插入数据
 * @return 插入后子树的根节点
 */
static Node *_insert_recursive(BST *bst, Node *node, const void *data)
{
    if (!node)
    {
        // 找到插入位置，创建新节点
        bst->size++;
        return _create_node(data, bst->element_size);
    }

    int cmp = bst->compare(data, node->data);
    if (cmp < 0)
    {
        // 数据更小，插入左子树
        node->left = _insert_recursive(bst, node->left, data);
    }
    else if (cmp > 0)
    {
        // 数据更大，插入右子树
        node->right = _insert_recursive(bst, node->right, data);
    }
    // 相等元素不插入（如需支持重复可修改此处逻辑）

    return node;
}

/**
 * 查找子树中最小节点（最左节点）
 *
 * @param node 子树根节点
 * @return 最小节点指针
 */
static Node *_find_min_node(Node *node)
{
    if (!node)
    {
        fprintf(stderr, "[error: _find_min_node] invalid arguments\n");
        return NULL;
    }
    while (node->left)
    {
        node = node->left;
    }
    return node;
}

/**
 * 递归删除节点
 *
 * @param bst BST指针
 * @param node 当前节点
 * @param key 待删除关键字
 * @return 删除后子树的根节点
 */
static Node *_remove_recursive(BST *bst, Node *node, const void *key)
{
    if (!node)
    {
        fprintf(stderr, "[error: _remove_recursive] invalid arguments\n");
        return NULL; // 未找到节点
    }

    // 1. 查找目标节点
    int cmp = bst->compare(key, node->data);
    if (cmp < 0)
    {
        node->left = _remove_recursive(bst, node->left, key);
    }
    else if (cmp > 0)
    {
        node->right = _remove_recursive(bst, node->right, key);
    }
    else
    {
        // 2. 找到目标节点，处理删除
        if (!node->left && !node->right)
        {
            // 情况1：叶子节点
            bst->size--;
            free(node->data);
            free(node);
            return NULL;
        }
        else if (!node->left)
        {
            // 情况2：只有右子树
            bst->size--;
            Node *temp = node->right;
            free(node->data);
            free(node);
            return temp;
        }
        else if (!node->right)
        {
            // 情况2：只有左子树
            bst->size--;
            Node *temp = node->left;
            free(node->data);
            free(node);
            return temp;
        }
        else
        {
            // 情况3：有两个子树（用后继节点替换）
            Node *successor = _find_min_node(node->right); // 找右子树最小节点
            memcpy(node->data, successor->data, bst->element_size); // 复制数据
            node->right = _remove_recursive(bst, node->right,
                                            successor->data); // 删除后继
        }
    }
    return node;
}

/**
 * 递归遍历
 *
 * @param node 当前节点
 * @param visit 访问函数
 * @param order 遍历顺序
 */
static void _traverse_recursive(const Node *node, VisitFunc visit,
                                TraverseOrder order)
{
    if (!node || !visit)
    {
        return;
    }

    if (order == PRE_ORDER)
    {
        visit(node->data); // 前序：先访问根
    }
    _traverse_recursive(node->left, visit, order);

    if (order == IN_ORDER)
    {
        visit(node->data); // 中序：中间访问根
    }
    _traverse_recursive(node->right, visit, order);

    if (order == POST_ORDER)
    {
        visit(node->data); // 后序：最后访问根
    }
}

// --- Public Interface ---

BST *bst_create(size_t element_size, CompareFunc compare_func)
{
    if (element_size == 0 || !compare_func)
    {
        fprintf(stderr, "[error: bst_create] invalid arguments\n");
        return NULL;
    }

    BST *bst = (BST *)malloc(sizeof(BST));
    if (!bst)
    {
        fprintf(stderr, "[error: bst_create] failed to allocate memory\n");
        return NULL;
    }

    bst->root = NULL;
    bst->element_size = element_size;
    bst->size = 0;
    bst->compare = compare_func;
    return bst;
}

void bst_destroy(BST **p_bst)
{
    if (!p_bst || !*p_bst)
    {
        fprintf(stderr, "[error: bst_destroy] invalid arguments\n");
        return;
    }

    _destroy_subtree((*p_bst)->root);
    free(*p_bst);
    *p_bst = NULL; // 置空外部指针，避免悬垂
}

void bst_clear(BST *bst)
{
    if (!bst)
    {
        fprintf(stderr, "[error: bst_clear] invalid arguments\n");
        return;
    }

    _destroy_subtree(bst->root);
    bst->root = NULL;
    bst->size = 0;
}

bool bst_insert(BST *bst, const void *element_data)
{
    if (!bst || !element_data)
    {
        return false;
    }

    size_t old_size = bst->size;
    bst->root = _insert_recursive(bst, bst->root, element_data);
    return bst->size > old_size;
}

bool bst_remove(BST *bst, const void *element_data)
{
    if (!bst || !element_data || bst_is_empty(bst))
    {
        return false;
    }

    size_t old_size = bst->size;
    bst->root = _remove_recursive(bst, bst->root, element_data);
    return bst->size < old_size;
}

bool bst_search(const BST *bst, const void *key)
{
    if (!bst || !key)
    {
        return false;
    }

    const Node *current = bst->root;
    while (current)
    {
        int cmp = bst->compare(key, current->data);
        if (cmp == 0)
        {
            return true;
        }
        current = (cmp < 0) ? current->left : current->right;
    }
    return false;
}

void bst_traverse(const BST *bst, TraverseOrder order, VisitFunc visit_func)
{
    if (bst && visit_func)
    {
        _traverse_recursive(bst->root, visit_func, order);
    }
}

bool bst_is_empty(const BST *bst)
{
    return !bst || bst->size == 0;
}

size_t bst_get_size(const BST *bst)
{
    return bst ? bst->size : 0;
}

