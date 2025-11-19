#pragma once

#include <stdbool.h> // for bool
#include <stddef.h>  // for size_t

// 向前声明: 隐藏内部实现细节, 降低耦合
typedef struct AVLNode AVLNode;
typedef struct AVLTree AVLTree;

/**
 * @brief 比较函数类型(用户自定义数据比较规则)
 *
 * @param a 待比较数据1
 * @param b 待比较数据2
 * @param context 比较上下文
 * @return <0: a < b; 0: a == b; >0: a > b
 */
typedef int (*AVLCompareFunc)(const void *a, const void *b, void *context);

/**
 * @brief 遍历回调函数类型(处理遍历到的节点数据)
 *
 * @param data 节点存储的数据
 * @param context 用户传递的上下文(如打印格式控制)
 */
typedef void (*AVLTraverseFunc)(void *data, void *context);

/**
 * @brief 数据释放回调函数类型(自定义数据内存释放逻辑)
 *
 * @param data 待释放的数据
 * @param context 用户传递的上下文
 */
typedef void (*AVLFreeFunc)(void *data, void *context);

/**
 * @brief 创建AVL树实例
 * @param compare 数据比较函数(必填, 不可为NULL)
 * @param free_func 数据释放函数(可选, NULL表示不自动释放数据)
 * @param context 上下文指针(传递给比较/释放/遍历函数)
 * @return 成功返回AVL树指针, 失败返回NULL
 */
AVLTree *avl_create(AVLCompareFunc compare, AVLFreeFunc free_func,
                    void *context);

/**
 * @brief 销毁AVL树(释放所有节点和数据)
 * @param tree 待销毁的AVL树(NULL安全)
 */
void avl_destroy(AVLTree *tree);

/**
 * @brief 插入数据到AVL树
 * @param tree AVL树实例(不可为NULL)
 * @param data 待插入的数据(不可为NULL)
 * @return 成功插入(数据不存在)返回true, 重复插入返回false
 */
bool avl_insert(AVLTree *tree, void *data);

/**
 * @brief 从AVL树删除指定数据
 * @param tree AVL树实例(不可为NULL)
 * @param data 待删除数据的匹配关键字(不可为NULL)
 * @return 成功删除返回true, 数据不存在返回false
 */
bool avl_delete(AVLTree *tree, const void *data);

/**
 * @brief 查找AVL树中的数据
 * @param tree AVL树实例(不可为NULL)
 * @param data 查找关键字(不可为NULL)
 * @return 找到返回对应数据指针, 未找到返回NULL
 */
void *avl_find(const AVLTree *tree, const void *data);

/**
 * @brief 更新AVL树中的数据(删除旧数据，插入新数据)
 * @param tree AVL树实例(不可为NULL)
 * @param old_data 旧数据的匹配关键字(不可为NULL)
 * @param new_data 新数据(不可为NULL)
 * @return 旧数据存在且更新成功返回true, 否则返回false
 */
bool avl_update(AVLTree *tree, const void *old_data, void *new_data);

/**
 * @brief 获取AVL树中元素个数
 * @param tree AVL树实例(不可为NULL)
 * @return 元素个数
 */
size_t avl_size(const AVLTree *tree);

/**
 * @brief 检查AVL树是否为空
 * @param tree AVL树实例(不可为NULL)
 * @return 空树返回true, 否则返回false
 */
bool avl_is_empty(const AVLTree *tree);

/**
 * @brief 中序遍历AVL树(升序输出, 符合BST特性)
 * @param tree AVL树实例(不可为NULL)
 * @param func 遍历回调函数(不可为NULL)
 * @param context 传递给回调函数的上下文
 */
void avl_traverse_inorder(const AVLTree *tree, AVLTraverseFunc func,
                          void *context);

/**
 * @brief 前序遍历AVL树(→左→右)
 * @param tree AVL树实例(不可为NULL)
 * @param func 遍历回调函数(不可为NULL)
 * @param context 传递给回调函数的上下文
 */
void avl_traverse_preorder(const AVLTree *tree, AVLTraverseFunc func,
                           void *context);

/**
 * @brief 后序遍历AVL树(左→右→根)
 * @param tree AVL树实例(不可为NULL)
 * @param func 遍历回调函数(不可为NULL)
 * @param context 传递给回调函数的上下文
 */
void avl_traverse_postorder(const AVLTree *tree, AVLTraverseFunc func,
                            void *context);

/**
 * @brief 获取AVL树的高度(空树高度为0)
 * @param tree AVL树实例(不可为NULL)
 * @return 树的高度
 */
int avl_height(const AVLTree *tree);

/**
 * @brief 验证AVL树的合法性(调试用)
 * @param tree AVL树实例(不可为NULL)
 * @return 符合AVL特性(BST+平衡因子≤1)返回true, 否则返回false
 */
bool avl_validate(const AVLTree *tree);

