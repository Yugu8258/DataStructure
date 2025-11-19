#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 颜色定义
typedef enum
{
    RB_BLACK = 0,
    RB_RED = 1
} rb_color_t;

// 红黑树节点结构
typedef struct rb_node
{
    void *key;
    void *value;
    struct rb_node *parent;
    struct rb_node *left;
    struct rb_node *right;
    rb_color_t color;
} rb_node_t;

// 比较函数类型
typedef int (*rb_compare_func)(const void *, const void *, void *context);

// 键值释放函数类型
typedef void (*rb_free_func)(void *data, void *context);

// 遍历回调函数类型
typedef void (*rb_traverse_func)(void *key, void *value, void *context);

// 红黑树上下文结构
typedef struct rb_context
{
    rb_compare_func compare;
    rb_free_func free_key;
    rb_free_func free_value;
    void *user_data;
    size_t key_size;
    size_t value_size;
} rb_context_t;

// 红黑树结构
typedef struct rb_tree
{
    rb_node_t *root;
    rb_node_t *nil; // 哨兵节点
    rb_context_t *context;
    size_t size;
} rb_tree_t;

// 创建节点
static rb_node_t *rb_node_create(rb_tree_t *tree, void *key, void *value)
{
    rb_node_t *node = (rb_node_t *)malloc(sizeof(rb_node_t));
    if (!node)
        return NULL;

    // 复制键
    node->key = malloc(tree->context->key_size);
    if (!node->key)
    {
        free(node);
        return NULL;
    }
    memcpy(node->key, key, tree->context->key_size);

    // 复制值
    node->value = malloc(tree->context->value_size);
    if (!node->value)
    {
        free(node->key);
        free(node);
        return NULL;
    }
    memcpy(node->value, value, tree->context->value_size);

    node->parent = tree->nil;
    node->left = tree->nil;
    node->right = tree->nil;
    node->color = RB_RED;

    return node;
}

// 释放节点
static void rb_node_free(rb_tree_t *tree, rb_node_t *node)
{
    if (node && node != tree->nil)
    {
        if (tree->context->free_key)
        {
            tree->context->free_key(node->key, tree->context->user_data);
        }
        else
        {
            free(node->key);
        }

        if (tree->context->free_value)
        {
            tree->context->free_value(node->value, tree->context->user_data);
        }
        else
        {
            free(node->value);
        }

        free(node);
    }
}

// 左旋
static void rb_rotate_left(rb_tree_t *tree, rb_node_t *x)
{

    // p-x-a-y-b-c

    /*
     *             P
     *             |
     *             X
     *           /   \
     *          a     y
     *              /   \
     *             b     c
     *
     *             P
     *             |
     *             y
     *           /   \
     *          x     c
     *        /   \
     *        a    b
     *
     */

    rb_node_t *y = x->right;
    x->right = y->left;

    if (y->left != tree->nil)
    {
        y->left->parent = x;
    }

    y->parent = x->parent;

    if (x->parent == tree->nil)
    {
        tree->root = y;
    }
    else if (x == x->parent->left)
    {
        x->parent->left = y;
    }
    else
    {
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
}

// 右旋
static void rb_rotate_right(rb_tree_t *tree, rb_node_t *y)
{
    rb_node_t *x = y->left;
    y->left = x->right;

    if (x->right != tree->nil)
    {
        x->right->parent = y;
    }

    x->parent = y->parent;

    if (y->parent == tree->nil)
    {
        tree->root = x;
    }
    else if (y == y->parent->right)
    {
        y->parent->right = x;
    }
    else
    {
        y->parent->left = x;
    }

    x->right = y;
    y->parent = x;
}

// 插入修复
static void rb_insert_fixup(rb_tree_t *tree, rb_node_t *z)
{
    while (z->parent->color == RB_RED)
    {
        if (z->parent == z->parent->parent->left)
        {
            rb_node_t *y = z->parent->parent->right;
            if (y->color == RB_RED)
            {
                z->parent->color = RB_BLACK;
                y->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->right)
                {
                    z = z->parent;
                    rb_rotate_left(tree, z);
                }
                z->parent->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                rb_rotate_right(tree, z->parent->parent);
            }
        }
        else
        {
            rb_node_t *y = z->parent->parent->left;
            if (y->color == RB_RED)
            {
                z->parent->color = RB_BLACK;
                y->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->left)
                {
                    z = z->parent;
                    rb_rotate_right(tree, z);
                }
                z->parent->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                rb_rotate_left(tree, z->parent->parent);
            }
        }
    }
    tree->root->color = RB_BLACK;
}

// 移植子树
static void rb_transplant(rb_tree_t *tree, rb_node_t *u, rb_node_t *v)
{
    if (u->parent == tree->nil)
    {
        tree->root = v;
    }
    else if (u == u->parent->left)
    {
        u->parent->left = v;
    }
    else
    {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

// 查找最小节点
static rb_node_t *rb_minimum(rb_tree_t *tree, rb_node_t *x)
{
    while (x->left != tree->nil)
    {
        x = x->left;
    }
    return x;
}

// 删除修复
static void rb_delete_fixup(rb_tree_t *tree, rb_node_t *x)
{
    while (x != tree->root && x->color == RB_BLACK)
    {
        if (x == x->parent->left)
        {
            rb_node_t *w = x->parent->right;
            if (w->color == RB_RED)
            {
                w->color = RB_BLACK;
                x->parent->color = RB_RED;
                rb_rotate_left(tree, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == RB_BLACK && w->right->color == RB_BLACK)
            {
                w->color = RB_RED;
                x = x->parent;
            }
            else
            {
                if (w->right->color == RB_BLACK)
                {
                    w->left->color = RB_BLACK;
                    w->color = RB_RED;
                    rb_rotate_right(tree, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = RB_BLACK;
                w->right->color = RB_BLACK;
                rb_rotate_left(tree, x->parent);
                x = tree->root;
            }
        }
        else
        {
            rb_node_t *w = x->parent->left;
            if (w->color == RB_RED)
            {
                w->color = RB_BLACK;
                x->parent->color = RB_RED;
                rb_rotate_right(tree, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == RB_BLACK && w->left->color == RB_BLACK)
            {
                w->color = RB_RED;
                x = x->parent;
            }
            else
            {
                if (w->left->color == RB_BLACK)
                {
                    w->right->color = RB_BLACK;
                    w->color = RB_RED;
                    rb_rotate_left(tree, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = RB_BLACK;
                w->left->color = RB_BLACK;
                rb_rotate_right(tree, x->parent);
                x = tree->root;
            }
        }
    }
    x->color = RB_BLACK;
}

// 创建红黑树
rb_tree_t *rb_tree_create(rb_context_t *context)
{
    if (!context || !context->compare)
        return NULL;

    rb_tree_t *tree = (rb_tree_t *)malloc(sizeof(rb_tree_t));
    if (!tree)
        return NULL;

    tree->nil = (rb_node_t *)malloc(sizeof(rb_node_t));
    if (!tree->nil)
    {
        free(tree);
        return NULL;
    }

    tree->nil->color = RB_BLACK;
    tree->nil->parent = tree->nil;
    tree->nil->left = tree->nil;
    tree->nil->right = tree->nil;
    tree->nil->key = NULL;
    tree->nil->value = NULL;

    tree->root = tree->nil;
    tree->context = context;
    tree->size = 0;

    return tree;
}

// 查找节点
static rb_node_t *rb_find_node(rb_tree_t *tree, void *key)
{
    rb_node_t *current = tree->root;

    while (current != tree->nil)
    {
        int cmp =
            tree->context->compare(key, current->key, tree->context->user_data);
        if (cmp == 0)
        {
            return current;
        }
        else if (cmp < 0)
        {
            current = current->left;
        }
        else
        {
            current = current->right;
        }
    }

    return NULL;
}

// 插入
bool rb_tree_insert(rb_tree_t *tree, void *key, void *value)
{
    if (!tree || !key || !value)
        return false;

    rb_node_t *z = rb_node_create(tree, key, value);
    if (!z)
        return false;

    rb_node_t *y = tree->nil;
    rb_node_t *x = tree->root;

    while (x != tree->nil)
    {
        y = x;
        int cmp =
            tree->context->compare(z->key, x->key, tree->context->user_data);
        if (cmp == 0)
        {
            // 键已存在，更新值
            if (tree->context->free_value)
            {
                tree->context->free_value(x->value, tree->context->user_data);
            }
            else
            {
                free(x->value);
            }
            x->value = malloc(tree->context->value_size);
            if (x->value)
            {
                memcpy(x->value, value, tree->context->value_size);
            }
            rb_node_free(tree, z);
            return true;
        }
        else if (cmp < 0)
        {
            x = x->left;
        }
        else
        {
            x = x->right;
        }
    }

    z->parent = y;
    if (y == tree->nil)
    {
        tree->root = z;
    }
    else if (tree->context->compare(z->key, y->key, tree->context->user_data) <
             0)
    {
        y->left = z;
    }
    else
    {
        y->right = z;
    }

    tree->size++;
    rb_insert_fixup(tree, z);
    return true;
}

// 查找
void *rb_tree_find(rb_tree_t *tree, void *key)
{
    if (!tree || !key)
        return NULL;

    rb_node_t *node = rb_find_node(tree, key);
    return node ? node->value : NULL;
}

// 删除
bool rb_tree_delete(rb_tree_t *tree, void *key)
{
    if (!tree || !key)
        return false;

    rb_node_t *z = rb_find_node(tree, key);
    if (!z)
        return false;

    rb_node_t *y = z;
    rb_node_t *x;
    rb_color_t y_original_color = y->color;

    if (z->left == tree->nil)
    {
        x = z->right;
        rb_transplant(tree, z, z->right);
    }
    else if (z->right == tree->nil)
    {
        x = z->left;
        rb_transplant(tree, z, z->left);
    }
    else
    {
        y = rb_minimum(tree, z->right);
        y_original_color = y->color;
        x = y->right;

        if (y->parent == z)
        {
            x->parent = y;
        }
        else
        {
            rb_transplant(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        rb_transplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (y_original_color == RB_BLACK)
    {
        rb_delete_fixup(tree, x);
    }

    rb_node_free(tree, z);
    tree->size--;
    return true;
}

// 中序遍历辅助函数
static void rb_inorder_traverse_helper(rb_tree_t *tree, rb_node_t *node,
                                       rb_traverse_func func, void *context)
{
    if (node != tree->nil)
    {
        rb_inorder_traverse_helper(tree, node->left, func, context);
        func(node->key, node->value, context);
        rb_inorder_traverse_helper(tree, node->right, func, context);
    }
}

// 中序遍历
void rb_tree_traverse(rb_tree_t *tree, rb_traverse_func func, void *context)
{
    if (tree && func)
    {
        rb_inorder_traverse_helper(tree, tree->root, func, context);
    }
}

// 清空树辅助函数
static void rb_clear_helper(rb_tree_t *tree, rb_node_t *node)
{
    if (node != tree->nil)
    {
        rb_clear_helper(tree, node->left);
        rb_clear_helper(tree, node->right);
        rb_node_free(tree, node);
    }
}

// 清空树
void rb_tree_clear(rb_tree_t *tree)
{
    if (tree)
    {
        rb_clear_helper(tree, tree->root);
        tree->root = tree->nil;
        tree->size = 0;
    }
}

// 销毁树
void rb_tree_destroy(rb_tree_t *tree)
{
    if (tree)
    {
        rb_tree_clear(tree);
        free(tree->nil);
        free(tree);
    }
}

// 获取树的大小
size_t rb_tree_size(rb_tree_t *tree)
{
    return tree ? tree->size : 0;
}

// 检查树是否为空
bool rb_tree_empty(rb_tree_t *tree)
{
    return !tree || tree->size == 0;
}

// ============ 测试代码 ============

// 测试用的上下文数据
typedef struct test_context
{
    int compare_count;
    int traverse_count;
} test_context_t;

// 整数比较函数
int int_compare(const void *a, const void *b, void *context)
{
    test_context_t *ctx = (test_context_t *)context;
    if (ctx)
        ctx->compare_count++;

    int ia = *(const int *)a;
    int ib = *(const int *)b;
    return (ia > ib) - (ia < ib);
}

// 遍历打印函数
void print_int_pair(void *key, void *value, void *context)
{
    test_context_t *ctx = (test_context_t *)context;
    if (ctx)
        ctx->traverse_count++;

    printf("Key: %d, Value: %d\n", *(int *)key, *(int *)value);
}

// 验证红黑树性质
static int verify_rb_properties_helper(rb_tree_t *tree, rb_node_t *node,
                                       int black_height)
{
    if (node == tree->nil)
    {
        return black_height;
    }

    // 检查红色节点的子节点必须是黑色
    if (node->color == RB_RED)
    {
        if (node->left->color == RB_RED || node->right->color == RB_RED)
        {
            printf("Error: Red node has red child!\n");
            return -1;
        }
    }

    if (node->color == RB_BLACK)
    {
        black_height++;
    }

    int left_height =
        verify_rb_properties_helper(tree, node->left, black_height);
    int right_height =
        verify_rb_properties_helper(tree, node->right, black_height);

    if (left_height == -1 || right_height == -1 || left_height != right_height)
    {
        printf("Error: Black height mismatch!\n");
        return -1;
    }

    return left_height;
}

bool verify_rb_properties(rb_tree_t *tree)
{
    if (!tree || tree->root == tree->nil)
        return true;

    // 检查根节点是否为黑色
    if (tree->root->color != RB_BLACK)
    {
        printf("Error: Root is not black!\n");
        return false;
    }

    // 检查黑高度
    return verify_rb_properties_helper(tree, tree->root, 0) != -1;
}

int main()
{
    printf("Red-Black Tree Test Program\n");
    printf("===========================\n\n");

    // 创建测试上下文
    test_context_t test_ctx = {0, 0};

    // 创建红黑树上下文
    rb_context_t ctx = {.compare = int_compare,
                        .free_key = NULL,
                        .free_value = NULL,
                        .user_data = &test_ctx,
                        .key_size = sizeof(int),
                        .value_size = sizeof(int)};

    // 创建红黑树
    rb_tree_t *tree = rb_tree_create(&ctx);
    if (!tree)
    {
        printf("Failed to create tree!\n");
        return 1;
    }

    // 测试插入
    printf("Test 1: Insertion\n");
    int keys[] = {10, 20, 30, 15, 25, 5, 1, 13, 17, 40, 7, 23};
    int values[] = {100, 200, 300, 150, 250, 50, 10, 130, 170, 400, 70, 230};
    int n = sizeof(keys) / sizeof(keys[0]);

    for (int i = 0; i < n; i++)
    {
        printf("Inserting key=%d, value=%d\n", keys[i], values[i]);
        if (!rb_tree_insert(tree, &keys[i], &values[i]))
        {
            printf("Failed to insert!\n");
            rb_tree_destroy(tree);
            return 1;
        }
    }

    printf("\nTree size: %zu\n", rb_tree_size(tree));
    printf("Compare operations: %d\n", test_ctx.compare_count);

    // 验证红黑树性质
    printf("\nVerifying Red-Black tree properties... ");
    if (verify_rb_properties(tree))
    {
        printf("PASSED\n");
    }
    else
    {
        printf("FAILED\n");
        rb_tree_destroy(tree);
        return 1;
    }

    // 测试查找
    printf("\nTest 2: Search\n");
    int search_keys[] = {15, 7, 100, 25};
    for (int i = 0; i < 4; i++)
    {
        int *found = (int *)rb_tree_find(tree, &search_keys[i]);
        if (found)
        {
            printf("Found key=%d, value=%d\n", search_keys[i], *found);
        }
        else
        {
            printf("Key=%d not found\n", search_keys[i]);
        }
    }

    // 测试更新（通过重新插入）
    printf("\nTest 3: Update\n");
    int update_key = 15;
    int new_value = 999;
    printf("Updating key=%d with new value=%d\n", update_key, new_value);
    rb_tree_insert(tree, &update_key, &new_value);

    int *updated = (int *)rb_tree_find(tree, &update_key);
    if (updated)
    {
        printf("Updated value: %d\n", *updated);
    }

    // 测试遍历
    printf("\nTest 4: Traversal (in-order)\n");
    test_ctx.traverse_count = 0;
    rb_tree_traverse(tree, print_int_pair, &test_ctx);
    printf("Traversed %d nodes\n", test_ctx.traverse_count);

    // 测试删除
    printf("\nTest 5: Deletion\n");
    int delete_keys[] = {13, 7, 30, 10};
    for (int i = 0; i < 4; i++)
    {
        printf("Deleting key=%d... ", delete_keys[i]);
        if (rb_tree_delete(tree, &delete_keys[i]))
        {
            printf("SUCCESS\n");
        }
        else
        {
            printf("FAILED (key not found)\n");
        }
    }

    printf("\nTree size after deletions: %zu\n", rb_tree_size(tree));

    // 再次验证红黑树性质
    printf("\nVerifying Red-Black tree properties after deletions... ");
    if (verify_rb_properties(tree))
    {
        printf("PASSED\n");
    }
    else
    {
        printf("FAILED\n");
    }

    // 最终遍历
    printf("\nFinal tree contents:\n");
    rb_tree_traverse(tree, print_int_pair, &test_ctx);

    // 测试清空
    printf("\nTest 6: Clear\n");
    rb_tree_clear(tree);
    printf("Tree size after clear: %zu\n", rb_tree_size(tree));
    printf("Is tree empty? %s\n", rb_tree_empty(tree) ? "Yes" : "No");

    // 清理
    rb_tree_destroy(tree);

    printf("\nAll tests completed!\n");
    printf("Total compare operations: %d\n", test_ctx.compare_count);

    return 0;
}

