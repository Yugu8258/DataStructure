// bst.h binary_search_tree

#pragma once

#include <stdbool.h> // for bool
#include <stddef.h>  // for size_t

// --- Opaque Pointer and Function Pointer Type Definitions ---

/**
 * 不透明指针: 隐藏BST内部实现, 仅暴露操作接口
 */
typedef struct BST BST;

/**
 * 比较函数指针: 定义元素间的大小元素 (BST核心)
 *
 * @param a 待比较元素1
 * @param b 待比较元素2
 * @return -1: a < b, 0: a == b, 1: a > b
 */
typedef int (*CompareFunc)(const void *a, const void *b);

/**
 * 访问函数指针：遍历节点时执行的操作
 *
 * @param data 节点存储的数据
 */
typedef void (*VisitFunc)(const void *data);

/**
 * 遍历顺序枚举
 */
typedef enum
{
    IN_ORDER,  // 中序遍历（左->根->右）：BST中序遍历结果为升序
    PRE_ORDER, // 前序遍历（根->左->右）：适合复制树结构
    POST_ORDER // 后序遍历（左->右->根）：适合销毁树
} TraverseOrder;

// --- Public API Prototypes ---

/**
 * 创建新的二叉搜索树
 *
 * @param element_size 元素大小（字节）
 * @param compare_func 比较函数（非NULL）
 * @return 成功返回BST指针，失败返回NULL
 */
BST *bst_create(size_t element_size, CompareFunc compare_func);

/**
 * 销毁二叉搜索树
 *
 * @param p_bst 指向BST指针的指针（用于置空外部指针）
 */
void bst_destroy(BST **p_bst);

/**
 * 清空树中所有节点（保留树结构可复用）
 *
 * @param bst BST指针
 */
void bst_clear(BST *bst);

/**
 * 插入元素
 *
 * @param bst BST指针
 * @param element_data 待插入元素地址
 * @return 成功返回true，失败（已存在或参数错误）返回false
 */
bool bst_insert(BST *bst, const void *element_data);

/**
 * 删除元素
 *
 * @param bst BST指针
 * @param element_data 待删除元素地址
 * @return 成功返回true，失败（不存在或参数错误）返回false
 */
bool bst_remove(BST *bst, const void *element_data);

/**
 * 查找元素
 *
 * @param bst BST指针
 * @param key 查找关键字地址
 * @return 存在返回true，否则返回false
 */
bool bst_search(const BST *bst, const void *key);

/**
 * 遍历树
 *
 * @param bst BST指针
 * @param order 遍历顺序
 * @param visit_func 访问函数
 */
void bst_traverse(const BST *bst, TraverseOrder order, VisitFunc visit_func);

/**
 * 判断树是否为空
 *
 * @param bst BST指针
 * @return 空返回true，否则返回false
 */
bool bst_is_empty(const BST *bst);

/**
 * 获取树的节点数量
 *
 * @param bst BST指针
 * @return 节点数量
 */
size_t bst_get_size(const BST *bst);

