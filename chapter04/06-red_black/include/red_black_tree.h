#pragma once

#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief 红黑树节点存储的数据类型（可按需修改，如int、float、结构体）
 * @note 若修改为结构体，需同步修改比较逻辑（如rb_tree_insert、rb_tree_search）
 */
typedef int RBData;

/**
 * @brief 节点颜色枚举
 */
typedef enum
{
    RB_BLACK, // 黑色节点
    RB_RED    // 红色节点
} RBColor;

/**
 * @brief 红黑树节点结构体
 */
typedef struct RBNode
{
    RBData key;            // 节点关键字（用于排序）
    RBColor color;         // 节点颜色
    struct RBNode *left;   // 左子节点
    struct RBNode *right;  // 右子节点
    struct RBNode *parent; // 父节点（便于旋转和调整）
} RBNode;

/**
 * @brief 红黑树结构体（存储根节点和NIL哨兵节点）
 * @note NIL节点：所有空指针均指向该哨兵节点，简化边界条件处理
 */
typedef struct
{
    RBNode *root; // 树根节点
    RBNode *nil;  // NIL哨兵节点（所有叶子节点和空指针的统一指向）
} RBTree;

// -------------------------- 红黑树核心API --------------------------

/**
 * @brief 创建一棵空的红黑树
 * @return 成功返回红黑树指针，失败返回NULL（内存分配失败）
 */
RBTree *rb_tree_create(void);

/**
 * @brief 销毁红黑树，释放所有节点内存（避免内存泄漏）
 * @param tree 红黑树指针（不可为NULL）
 */
void rb_tree_destroy(RBTree *tree);

/**
 * @brief 向红黑树中插入一个关键字
 * @param tree 红黑树指针（不可为NULL）
 * @param key 待插入的关键字（若已存在则不重复插入）
 * @return 成功返回0，失败返回-1（内存分配失败/参数无效）
 */
int rb_tree_insert(RBTree *tree, RBData key);

/**
 * @brief 从红黑树中删除一个关键字
 * @param tree 红黑树指针（不可为NULL）
 * @param key 待删除的关键字
 * @return 成功删除返回0，失败返回-1（关键字不存在/参数无效）
 */
int rb_tree_delete(RBTree *tree, RBData key);

/**
 * @brief 在红黑树中查找关键字
 * @param tree 红黑树指针（不可为NULL）
 * @param key 待查找的关键字
 * @return 找到返回节点指针，未找到返回NULL
 */
RBNode *rb_tree_search(const RBTree *tree, RBData key);

/**
 * @brief 中序遍历红黑树（升序输出关键字，验证二叉搜索树特性）
 * @param tree 红黑树指针（不可为NULL）
 * @param callback 遍历回调函数（用于处理每个节点的关键字）
 */
void rb_tree_inorder_traversal(const RBTree *tree,
                               void (*callback)(RBData key));

/**
 * @brief 获取红黑树的高度（根节点到最远叶子节点的路径长度）
 * @param tree 红黑树指针（不可为NULL）
 * @return 树的高度（空树返回0）
 */
size_t rb_tree_get_height(const RBTree *tree);

/**
 * @brief 验证红黑树的5条规则（用于调试，确保树结构合法）
 * @param tree 红黑树指针（不可为NULL）
 * @return 符合规则返回true，违反规则返回false
 */
bool rb_tree_validate(const RBTree *tree);

/**
 * @brief 打印红黑树的结构（层序遍历，标注节点颜色，用于调试）
 * @param tree 红黑树指针（不可为NULL）
 */
void rb_tree_print(const RBTree *tree);

/**
 * @brief 判断红黑树是否为空
 * @param tree 红黑树指针（不可为NULL）
 * @return 空返回true，否则返回false
 */
bool rb_tree_is_empty(const RBTree *tree);

