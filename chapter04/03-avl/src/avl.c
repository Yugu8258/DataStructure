// avl.c
#include "avl.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// AVL节点结构(内部实现，用户不可见)
struct AVLNode
{
    void *data;     // 存储用户数据
    AVLNode *left;  // 左子节点
    AVLNode *right; // 右子节点
    int height;     // 节点高度（叶子节点为1）
};

// AVL树结构(管理树的全局状态)
struct AVLTree
{
    AVLNode *root;          // 根节点
    AVLCompareFunc compare; // 数据比较函数
    AVLFreeFunc free_func;  // 数据释放函数
    void *context;          // 上下文指针
    size_t size;            // 元素个数
};

// 内部辅助函数声明(静态函数，仅当前文件可见)

/**
 * @brief 创建单个AVL节点
 *
 * @param data 节点数据
 * @return 创建的节点
 */
static AVLNode *_node_create(void *data);

/**
 * @brief 销毁单个AVL节点
 *
 * @param node 要销毁的节点
 * @param free_func 数据释放函数
 * @param context 上下文
 */
static void _node_destroy(AVLNode *node, AVLFreeFunc free_func, void *context);

/**
 * @brief 获取节点高度(递归计算)
 *
 * @param node 要计算的节点
 * @return 节点高度
 */
static int _node_height(const AVLNode *node);

/**
 * @brief 更新节点高度(递归计算)
 *
 * @param node 要更新的节点
 */
static void _node_update_height(AVLNode *node);

/**
 * @brief 获取节点平衡因子
 *
 * @param node 要计算的节点
 * @return 平衡因子
 */
static int _node_balance_factor(const AVLNode *node);

/**
 * @brief 左旋(递归实现)
 *
 * @param node 要旋转的节点
 * @return 旋转后的节点
 */
static AVLNode *_node_rotate_left(AVLNode *node);

/**
 * @brief 右旋(递归实现)
 *
 * @param node 要旋转的节点
 * @return 旋转后的节点
 */
static AVLNode *_node_rotate_right(AVLNode *node);

/**
 * @brief 平衡节点(递归实现)
 *
 * @param node 要平衡的节点
 * @return 平衡后的节点
 */
static AVLNode *_node_balance(AVLNode *node);

/**
 * @brief 递归插入节点(遵循BST规则, 插入后平衡)
 *
 * @param node 要插入的节点
 * @param data 要插入的数据
 * @param compare 数据比较函数
 * @param context 上下文
 * @param inserted 插入成功标志
 * @return 插入后的节点
 */
static AVLNode *_node_insert(AVLNode *node, void *data, AVLCompareFunc compare,
                             void *context, bool *inserted);

/**
 * @brief 递归删除节点(遵循BST规则, 删除后平衡)
 *
 * @param node 要删除的节点
 * @param data 要删除的数据
 * @param compare 数据比较函数
 * @param context 上下文
 * @param free_func 数据释放函数
 * @param deleted 删除成功标志
 * @return 删除后的节点
 */
static AVLNode *_node_delete(AVLNode *node, const void *data,
                             AVLCompareFunc compare, void *context,
                             AVLFreeFunc free_func, bool *deleted);

/**
 * @brief 查找最小节点(递归实现)
 *
 * @param node 要查找的节点
 * @return 最小节点
 */
static AVLNode *_node_find_min(AVLNode *node);

/**
 * @brief 查找节点(递归实现)
 *
 * @param node 要查找的节点
 * @param data 要查找的数据
 * @param compare 数据比较函数
 * @param context 上下文
 */
static void *_node_find(const AVLNode *node, const void *data,
                        AVLCompareFunc compare, void *context);

/**
 * @brief 递归中序遍历
 *
 * @param node 要遍历的节点
 * @param func 遍历函数
 * @param context 上下文
 */
static void _node_traverse_inorder(const AVLNode *node, AVLTraverseFunc func,
                                   void *context);

/**
 * @brief 递归前序遍历
 *
 * @param node 要遍历的节点
 * @param func 遍历函数
 * @param context 上下文
 */
static void _node_traverse_preorder(const AVLNode *node, AVLTraverseFunc func,
                                    void *context);

/**
 * @brief 递归后序遍历
 *
 * @param node 要遍历的节点
 * @param func 遍历函数
 * @param context 上下文
 */
static void _node_traverse_postorder(const AVLNode *node, AVLTraverseFunc func,
                                     void *context);

/**
 * @brief 验证节点(递归检查BST特性和平衡因子)
 *
 * @param node 要验证的节点
 * @param compare 数据比较函数
 * @param context 上下文
 * @param min 最小值
 * @paran max 最大值
 * @return 验证结果
 */
static bool _node_validate(const AVLNode *node, AVLCompareFunc compare,
                           void *context, const void *min, const void *max);

// 外部接口实现

/**
 * @brief 创建AVL树实例
 */
AVLTree *avl_create(AVLCompareFunc compare, AVLFreeFunc free_func,
                    void *context)
{
    // 比较函数是必需的, 断言防止NULL
    assert(compare != NULL && "compare function cannot be NULL");

    AVLTree *tree = (AVLTree *)malloc(sizeof(AVLTree));
    if (tree == NULL)
    {
        fprintf(stderr, "[ERROR: avl_create] malloc failed\n");
        return NULL; // 内存分配失败
    }

    // 初始化树状态
    tree->root = NULL;
    tree->compare = compare;
    tree->free_func = free_func;
    tree->context = context;
    tree->size = 0;

    return tree;
}

/**
 * @brief 销毁AVL树(递归释放所有节点和数据)
 */
void avl_destroy(AVLTree *tree)
{
    if (tree == NULL)
    {
        return; // NULL安全
    }

    // 递归销毁所有节点
    _node_destroy(tree->root, tree->free_func, tree->context);
    free(tree); // 释放树结构本身
}

/**
 * @brief 插入数据到AVL树
 */
bool avl_insert(AVLTree *tree, void *data)
{
    assert(tree != NULL && "tree cannot be NULL");
    assert(data != NULL && "data cannot be NULL");

    bool inserted = false;
    // 递归插入节点并平衡树
    tree->root =
        _node_insert(tree->root, data, tree->compare, tree->context, &inserted);

    if (inserted)
    {
        tree->size++; // 插入成功, 更新元素个数
    }

    return inserted;
}

/**
 * @brief 从AVL树删除指定数据
 */
bool avl_delete(AVLTree *tree, const void *data)
{
    assert(tree != NULL && "tree cannot be NULL");
    assert(data != NULL && "data cannot be NULL");

    bool deleted = false;
    // 递归删除节点并平衡树
    tree->root = _node_delete(tree->root, data, tree->compare, tree->context,
                              tree->free_func, &deleted);

    if (deleted)
    {
        tree->size--; // 删除成功, 更新元素个数
    }

    return deleted;
}

/**
 * @brief 查找AVL树中的数据
 */
void *avl_find(const AVLTree *tree, const void *data)
{
    assert(tree != NULL && "tree cannot be NULL");
    assert(data != NULL && "data cannot be NULL");

    // 递归查找节点
    return _node_find(tree->root, data, tree->compare, tree->context);
}

/**
 * @brief 更新AVL树中的数据
 */
bool avl_update(AVLTree *tree, const void *old_data, void *new_data)
{
    assert(tree != NULL && "tree cannot be NULL");
    assert(old_data != NULL && "old_data cannot be NULL");
    assert(new_data != NULL && "new_data cannot be NULL");

    // 新旧数据相同，直接返回成功
    if (tree->compare(old_data, new_data, tree->context) == 0)
    {
        return true;
    }

    // 检查旧数据是否存在
    if (avl_find(tree, old_data) == NULL)
    {
        return false;
    }

    // 先删除旧数据，再插入新数据
    if (!avl_delete(tree, old_data))
    {
        return false;
    }

    return avl_insert(tree, new_data);
}

/**
 * @brief 获取AVL树中元素个数
 */
size_t avl_size(const AVLTree *tree)
{
    assert(tree != NULL && "tree cannot be NULL");
    return tree->size;
}

/**
 * @brief 检查AVL树是否为空
 */
bool avl_is_empty(const AVLTree *tree)
{
    assert(tree != NULL && "tree cannot be NULL");
    return tree->size == 0;
}

/**
 * @brief 中序遍历AVL树
 */
void avl_traverse_inorder(const AVLTree *tree, AVLTraverseFunc func,
                          void *context)
{
    assert(tree != NULL && "tree cannot be NULL");
    assert(func != NULL && "traverse function cannot be NULL");

    _node_traverse_inorder(tree->root, func, context);
}

/**
 * @brief 前序遍历AVL树
 */
void avl_traverse_preorder(const AVLTree *tree, AVLTraverseFunc func,
                           void *context)
{
    assert(tree != NULL && "tree cannot be NULL");
    assert(func != NULL && "traverse function cannot be NULL");

    _node_traverse_preorder(tree->root, func, context);
}

/**
 * @brief 后序遍历AVL树
 */
void avl_traverse_postorder(const AVLTree *tree, AVLTraverseFunc func,
                            void *context)
{
    assert(tree != NULL && "tree cannot be NULL");
    assert(func != NULL && "traverse function cannot be NULL");

    _node_traverse_postorder(tree->root, func, context);
}

/**
 * @brief 获取AVL树的高度
 */
int avl_height(const AVLTree *tree)
{
    assert(tree != NULL && "tree cannot be NULL");
    return _node_height(tree->root);
}

/**
 * @brief 验证AVL树的合法性
 */
bool avl_validate(const AVLTree *tree)
{
    assert(tree != NULL && "tree cannot be NULL");
    // 验证BST特性和平衡因子
    return _node_validate(tree->root, tree->compare, tree->context, NULL, NULL);
}

// -------------------------- 内部辅助函数实现 --------------------------

static AVLNode *_node_create(void *data)
{
    AVLNode *node = (AVLNode *)malloc(sizeof(AVLNode));
    if (node == NULL)
    {
        return NULL;
    }

    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->height = 1; // 新节点默认为叶子节点，高度1

    return node;
}

/**
 * @brief 递归销毁节点及其子树
 */
static void _node_destroy(AVLNode *node, AVLFreeFunc free_func, void *context)
{
    if (node == NULL)
    {
        return;
    }

    // 后序遍历销毁：先销毁子节点，再销毁当前节点
    _node_destroy(node->left, free_func, context);
    _node_destroy(node->right, free_func, context);

    // 如果用户提供了释放函数，调用它释放数据
    if (free_func != NULL)
    {
        free_func(node->data, context);
    }

    free(node); // 释放节点本身
}

/**
 * @brief 获取节点高度(空节点高度为0)
 */
static int _node_height(const AVLNode *node)
{
    return node != NULL ? node->height : 0;
}

/**
 * @brief 更新节点高度(基于左右子树高度)
 */
static void _node_update_height(AVLNode *node)
{
    if (node == NULL)
    {
        return;
    }

    // 节点高度 = 1 + 左右子树高度的最大值
    int left_h = _node_height(node->left);
    int right_h = _node_height(node->right);
    node->height = 1 + (left_h > right_h ? left_h : right_h);
}

/**
 * @brief 计算节点的平衡因子(左高 - 右高)
 */
static int _node_balance_factor(const AVLNode *node)
{
    if (node == NULL)
    {
        return 0;
    }
    return _node_height(node->left) - _node_height(node->right);
}

/**
 * @brief 左旋操作(处理RR型失衡)
 * @param node 失衡的根节点
 * @return 旋转后的新根节点
 */
static AVLNode *_node_rotate_left(AVLNode *node)
{
    assert(node != NULL && node->right != NULL &&
           "invalid rotate left: node or right child is NULL");

    AVLNode *new_root = node->right; // 新根节点是原节点的右子节点
    AVLNode *temp = new_root->left;  // 临时保存新根的左子树

    // 执行旋转
    new_root->left = node;
    node->right = temp;

    // 更新旋转后节点的高度(先更新下层节点)
    _node_update_height(node);
    _node_update_height(new_root);

    return new_root;
}

/**
 * @brief 右旋操作(处理LL型失衡)
 * @param node 失衡的根节点
 * @return 旋转后的新根节点
 */
static AVLNode *_node_rotate_right(AVLNode *node)
{
    assert(node != NULL && node->left != NULL &&
           "invalid rotate right: node or left child is NULL");

    AVLNode *new_root = node->left;  // 新根节点是原节点的左子节点
    AVLNode *temp = new_root->right; // 临时保存新根的右子树

    // 执行旋转
    new_root->right = node;
    node->left = temp;

    // 更新旋转后节点的高度(先更新下层节点)
    _node_update_height(node);
    _node_update_height(new_root);

    return new_root;
}

/**
 * @brief 平衡节点(处理四种失衡场景)
 */
static AVLNode *_node_balance(AVLNode *node)
{
    if (node == NULL)
    {
        return NULL;
    }

    // 先更新当前节点高度(子树可能已变化)
    _node_update_height(node);
    int bf = _node_balance_factor(node); // 计算平衡因子

    // 场景1: LL型失衡(左重, 左子树也左重)
    if (bf > 1 && _node_balance_factor(node->left) >= 0)
    {
        return _node_rotate_right(node);
    }

    // 场景2：LR型失衡(左重，左子树右重)
    if (bf > 1 && _node_balance_factor(node->left) < 0)
    {
        node->left = _node_rotate_left(node->left); // 先左旋左子树，转为LL型
        return _node_rotate_right(node);            // 再右旋当前节点
    }

    // 场景3：RR型失衡(右重，右子树也右重)
    if (bf < -1 && _node_balance_factor(node->right) <= 0)
    {
        return _node_rotate_left(node);
    }

    // 场景4：RL型失衡(右重，右子树左重)
    if (bf < -1 && _node_balance_factor(node->right) > 0)
    {
        node->right = _node_rotate_right(node->right); // 先右旋右子树，转为RR型
        return _node_rotate_left(node);                // 再左旋当前节点
    }

    // 未失衡，直接返回原节点
    return node;
}

/**
 * @brief 递归插入节点(遵循BST规则，插入后平衡)
 */
static AVLNode *_node_insert(AVLNode *node, void *data, AVLCompareFunc compare,
                             void *context, bool *inserted)
{
    // 找到插入位置(空节点)
    if (node == NULL)
    {
        *inserted = true;
        return _node_create(data);
    }

    int cmp = compare(data, node->data, context);

    if (cmp < 0)
    {
        // 数据小于当前节点，插入左子树
        node->left = _node_insert(node->left, data, compare, context, inserted);
    }
    else if (cmp > 0)
    {
        // 数据大于当前节点，插入右子树
        node->right =
            _node_insert(node->right, data, compare, context, inserted);
    }
    else
    {
        // 数据已存在，不插入
        *inserted = false;
        return node;
    }

    // 插入后平衡节点
    return _node_balance(node);
}

/**
 * @brief 查找节点的中序后继(右子树的最小值节点)
 */
static AVLNode *_node_find_min(AVLNode *node)
{
    // 一直向左遍历，直到左子节点为空
    while (node != NULL && node->left != NULL)
    {
        node = node->left;
    }
    return node;
}

/**
 * @brief 递归删除节点(遵循BST规则，删除后平衡)
 */
static AVLNode *_node_delete(AVLNode *node, const void *data,
                             AVLCompareFunc compare, void *context,
                             AVLFreeFunc free_func, bool *deleted)
{
    if (node == NULL)
    {
        *deleted = false;
        return NULL;
    }

    int cmp = compare(data, node->data, context);

    if (cmp < 0)
    {
        // 数据在左子树，递归删除
        node->left = _node_delete(node->left, data, compare, context, free_func,
                                  deleted);
    }
    else if (cmp > 0)
    {
        // 数据在右子树，递归删除
        node->right = _node_delete(node->right, data, compare, context,
                                   free_func, deleted);
    }
    else
    {
        // 找到待删除节点
        *deleted = true;

        // 情况1：叶子节点或只有一个子节点
        if (node->left == NULL || node->right == NULL)
        {
            AVLNode *temp = node->left != NULL ? node->left : node->right;

            // 释放当前节点的数据和节点本身
            if (free_func != NULL)
            {
                free_func(node->data, context);
            }
            free(node);

            return temp; // 返回子节点(可能为NULL)
        }

        // 情况2：有两个子节点——用中序后继(右子树最小值)替换
        AVLNode *successor = _node_find_min(node->right);
        // 交换数据（避免直接删除节点导致树结构混乱）
        void *temp_data = node->data;
        node->data = successor->data;
        successor->data = temp_data;

        // 递归删除后继节点(此时后继节点必然是叶子或只有一个子节点)
        node->right = _node_delete(node->right, temp_data, compare, context,
                                   free_func, deleted);
    }

    // 删除后平衡节点
    return _node_balance(node);
}

/**
 * @brief 递归查找节点
 */
static void *_node_find(const AVLNode *node, const void *data,
                        AVLCompareFunc compare, void *context)
{
    if (node == NULL)
    {
        return NULL;
    }

    int cmp = compare(data, node->data, context);

    if (cmp < 0)
    {
        return _node_find(node->left, data, compare, context);
    }
    else if (cmp > 0)
    {
        return _node_find(node->right, data, compare, context);
    }
    else
    {
        return node->data; // 找到，返回数据指针
    }
}

/**
 * @brief 递归中序遍历
 */
static void _node_traverse_inorder(const AVLNode *node, AVLTraverseFunc func,
                                   void *context)
{
    if (node == NULL)
    {
        return;
    }
    _node_traverse_inorder(node->left, func, context);
    func(node->data, context); // 访问当前节点
    _node_traverse_inorder(node->right, func, context);
}

/**
 * @brief 递归前序遍历
 */
static void _node_traverse_preorder(const AVLNode *node, AVLTraverseFunc func,
                                    void *context)
{
    if (node == NULL)
    {
        return;
    }
    func(node->data, context); // 访问当前节点
    _node_traverse_preorder(node->left, func, context);
    _node_traverse_preorder(node->right, func, context);
}

/**
 * @brief 递归后序遍历
 */
static void _node_traverse_postorder(const AVLNode *node, AVLTraverseFunc func,
                                     void *context)
{
    if (node == NULL)
    {
        return;
    }
    _node_traverse_postorder(node->left, func, context);
    _node_traverse_postorder(node->right, func, context);
    func(node->data, context); // 访问当前节点
}

/**
 * @brief 验证节点(递归检查BST特性和平衡因子)
 */
static bool _node_validate(const AVLNode *node, AVLCompareFunc compare,
                           void *context, const void *min, const void *max)
{
    if (node == NULL)
    {
        return true;
    }

    // 检查BST特性：当前节点值必须在[min, max)范围内
    if (min != NULL && compare(node->data, min, context) <= 0)
    {
        return false;
    }
    if (max != NULL && compare(node->data, max, context) >= 0)
    {
        return false;
    }

    // 检查平衡因子(绝对值≤1)
    int bf = _node_balance_factor(node);
    if (bf < -1 || bf > 1)
    {
        return false;
    }

    // 检查高度计算是否正确
    int expected_h = 1 + (_node_height(node->left) > _node_height(node->right)
                              ? _node_height(node->left)
                              : _node_height(node->right));
    if (node->height != expected_h)
    {
        return false;
    }

    // 递归验证左右子树
    return _node_validate(node->left, compare, context, min, node->data) &&
           _node_validate(node->right, compare, context, node->data, max);
}

