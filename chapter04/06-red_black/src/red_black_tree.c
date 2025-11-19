#include "red_black_tree.h"

#include <stdio.h>
#include <string.h>

// -------------------------- 静态辅助函数声明 --------------------------

static RBNode *rb_node_create(RBData key, RBColor color, RBNode *nil);
static void rb_left_rotate(RBTree *tree, RBNode *x);
static void rb_right_rotate(RBTree *tree, RBNode *y);
static void rb_insert_fixup(RBTree *tree, RBNode *z);
static RBNode *rb_tree_minimum(RBNode *node, RBNode *nil);
static void rb_delete_fixup(RBTree *tree, RBNode *x);
static void rb_tree_destroy_recursive(RBNode *node, RBNode *nil);
static bool rb_validate_recursive(const RBTree *tree, RBNode *node,
                                  size_t black_count,
                                  size_t *expected_black_count);
static size_t rb_tree_get_height_recursive(RBNode *node, RBNode *nil);
static void rb_tree_print_level(RBNode *node, RBNode *nil, size_t level,
                                size_t max_level);
static void rb_inorder_recursive(RBNode *node, RBNode *nil,
                                 void (*callback)(RBData key));

// -------------------------- 辅助函数实现 --------------------------

static RBNode *rb_node_create(RBData key, RBColor color, RBNode *nil)
{
    RBNode *node = (RBNode *)malloc(sizeof(RBNode));
    if (!node)
    {
        fprintf(stderr, "Error: Failed to allocate RBNode\n");
        return NULL;
    }
    node->key = key;
    node->color = color;
    node->parent = nil;
    node->left = nil;
    node->right = nil;
    return node;
}

static void rb_left_rotate(RBTree *tree, RBNode *x)
{
    RBNode *y = x->right;
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

static void rb_right_rotate(RBTree *tree, RBNode *y)
{
    RBNode *x = y->left;
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
    else if (y == y->parent->left)
    {
        y->parent->left = x;
    }
    else
    {
        y->parent->right = x;
    }

    x->right = y;
    y->parent = x;
}

static void rb_insert_fixup(RBTree *tree, RBNode *z)
{
    while (z->parent->color == RB_RED)
    {
        if (z->parent == z->parent->parent->left)
        {
            RBNode *y = z->parent->parent->right;
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
                    rb_left_rotate(tree, z);
                }
                z->parent->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                rb_right_rotate(tree, z->parent->parent);
            }
        }
        else
        {
            RBNode *y = z->parent->parent->left;
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
                    rb_right_rotate(tree, z);
                }
                z->parent->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                rb_left_rotate(tree, z->parent->parent);
            }
        }
    }
    tree->root->color = RB_BLACK;
}

static RBNode *rb_tree_minimum(RBNode *node, RBNode *nil)
{
    while (node->left != nil)
    {
        node = node->left;
    }
    return node;
}

static void rb_delete_fixup(RBTree *tree, RBNode *x)
{
    while (x != tree->root && x->color == RB_BLACK)
    {
        if (x == x->parent->left)
        {
            RBNode *w = x->parent->right;
            if (w->color == RB_RED)
            {
                w->color = RB_BLACK;
                x->parent->color = RB_RED;
                rb_left_rotate(tree, x->parent);
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
                    rb_right_rotate(tree, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = RB_BLACK;
                w->right->color = RB_BLACK;
                rb_left_rotate(tree, x->parent);
                x = tree->root;
            }
        }
        else
        {
            RBNode *w = x->parent->left;
            if (w->color == RB_RED)
            {
                w->color = RB_BLACK;
                x->parent->color = RB_RED;
                rb_right_rotate(tree, x->parent);
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
                    rb_left_rotate(tree, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = RB_BLACK;
                w->left->color = RB_BLACK;
                rb_right_rotate(tree, x->parent);
                x = tree->root;
            }
        }
    }
    x->color = RB_BLACK;
}

static void rb_tree_destroy_recursive(RBNode *node, RBNode *nil)
{
    if (node == nil)
        return;
    rb_tree_destroy_recursive(node->left, nil);
    rb_tree_destroy_recursive(node->right, nil);
    free(node);
}

static bool rb_validate_recursive(const RBTree *tree, RBNode *node,
                                  size_t black_count,
                                  size_t *expected_black_count)
{
    if (node == tree->nil)
    {
        if (*expected_black_count == 0)
        {
            *expected_black_count = black_count;
        }
        else if (black_count != *expected_black_count)
        {
            fprintf(stderr,
                    "Invalid RBTree: Black height inconsistent (got %zu, "
                    "expected %zu)\n",
                    black_count, *expected_black_count);
            return false;
        }
        return true;
    }

    if (node->color == RB_RED)
    {
        if (node->left->color == RB_RED || node->right->color == RB_RED)
        {
            fprintf(stderr, "Invalid RBTree: Red node has red child (key=%d)\n",
                    node->key);
            return false;
        }
    }
    else
    {
        black_count++;
    }

    if (node->left != tree->nil && node->left->key >= node->key)
    {
        fprintf(stderr, "Invalid RBTree: BST property violated (left child key "
                        ">= current key)\n");
        return false;
    }
    if (node->right != tree->nil && node->right->key <= node->key)
    {
        fprintf(stderr, "Invalid RBTree: BST property violated (right child "
                        "key <= current key)\n");
        return false;
    }

    return rb_validate_recursive(tree, node->left, black_count,
                                 expected_black_count) &&
           rb_validate_recursive(tree, node->right, black_count,
                                 expected_black_count);
}

static size_t rb_tree_get_height_recursive(RBNode *node, RBNode *nil)
{
    if (node == nil)
        return 0;
    size_t left = rb_tree_get_height_recursive(node->left, nil);
    size_t right = rb_tree_get_height_recursive(node->right, nil);
    return (left > right ? left : right) + 1;
}

static void rb_tree_print_level(RBNode *node, RBNode *nil, size_t level,
                                size_t max_level)
{
    if (node == nil || level > max_level)
        return;
    if (level == max_level)
    {
        const char *color_str = (node->color == RB_RED) ? "R" : "B";
        printf("%3d(%s) ", node->key, color_str);
    }
    else
    {
        rb_tree_print_level(node->left, nil, level + 1, max_level);
        rb_tree_print_level(node->right, nil, level + 1, max_level);
    }
}

static void rb_inorder_recursive(RBNode *node, RBNode *nil,
                                 void (*callback)(RBData key))
{
    if (node == nil)
        return;
    rb_inorder_recursive(node->left, nil, callback);
    callback(node->key);
    rb_inorder_recursive(node->right, nil, callback);
}

// -------------------------- 公共API实现 --------------------------

RBTree *rb_tree_create(void)
{
    RBTree *tree = (RBTree *)malloc(sizeof(RBTree));
    if (!tree)
    {
        fprintf(stderr, "Error: Failed to allocate RBTree\n");
        return NULL;
    }

    tree->nil = rb_node_create(0, RB_BLACK, NULL);
    if (!tree->nil)
    {
        free(tree);
        return NULL;
    }
    tree->nil->parent = tree->nil;
    tree->root = tree->nil;

    return tree;
}

void rb_tree_destroy(RBTree *tree)
{
    if (!tree)
        return;
    rb_tree_destroy_recursive(tree->root, tree->nil);
    free(tree->nil);
    free(tree);
}

int rb_tree_insert(RBTree *tree, RBData key)
{
    if (!tree)
        return -1;

    RBNode *y = tree->nil;
    RBNode *x = tree->root;
    while (x != tree->nil)
    {
        y = x;
        if (key < x->key)
        {
            x = x->left;
        }
        else if (key > x->key)
        {
            x = x->right;
        }
        else
        {
            return 0; // already exists
        }
    }

    RBNode *z = rb_node_create(key, RB_RED, tree->nil);
    if (!z)
        return -1;
    z->parent = y;

    if (y == tree->nil)
    {
        tree->root = z;
    }
    else if (z->key < y->key)
    {
        y->left = z;
    }
    else
    {
        y->right = z;
    }

    rb_insert_fixup(tree, z);
    return 0;
}

int rb_tree_delete(RBTree *tree, RBData key)
{
    if (!tree)
        return -1;

    RBNode *z = rb_tree_search(tree, key);
    if (z == NULL)
        return -1;

    RBNode *y = z;
    RBNode *x = tree->nil;
    RBColor y_original_color = y->color;

    if (z->left == tree->nil)
    {
        x = z->right;
        if (z->parent == tree->nil)
        {
            tree->root = x;
        }
        else if (z == z->parent->left)
        {
            z->parent->left = x;
        }
        else
        {
            z->parent->right = x;
        }
        x->parent = z->parent;
    }
    else if (z->right == tree->nil)
    {
        x = z->left;
        if (z->parent == tree->nil)
        {
            tree->root = x;
        }
        else if (z == z->parent->left)
        {
            z->parent->left = x;
        }
        else
        {
            z->parent->right = x;
        }
        x->parent = z->parent;
    }
    else
    {
        y = rb_tree_minimum(z->right, tree->nil);
        y_original_color = y->color;
        x = y->right;

        if (y->parent == z)
        {
            x->parent = y;
        }
        else
        {
            if (y->parent == tree->nil)
            {
                tree->root = x;
            }
            else if (y == y->parent->left)
            {
                y->parent->left = x;
            }
            else
            {
                y->parent->right = x;
            }
            x->parent = y->parent;
            y->right = z->right;
            y->right->parent = y;
        }

        if (z->parent == tree->nil)
        {
            tree->root = y;
        }
        else if (z == z->parent->left)
        {
            z->parent->left = y;
        }
        else
        {
            z->parent->right = y;
        }
        y->parent = z->parent;
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (y_original_color == RB_BLACK)
    {
        rb_delete_fixup(tree, x);
    }

    free(z);
    return 0;
}

RBNode *rb_tree_search(const RBTree *tree, RBData key)
{
    if (!tree)
        return NULL;

    RBNode *current = tree->root;
    while (current != tree->nil)
    {
        if (key < current->key)
        {
            current = current->left;
        }
        else if (key > current->key)
        {
            current = current->right;
        }
        else
        {
            return current;
        }
    }
    return NULL;
}

void rb_tree_inorder_traversal(const RBTree *tree, void (*callback)(RBData key))
{
    if (!tree || !callback)
        return;
    rb_inorder_recursive(tree->root, tree->nil, callback);
}

size_t rb_tree_get_height(const RBTree *tree)
{
    if (!tree)
        return 0;
    return rb_tree_get_height_recursive(tree->root, tree->nil);
}

bool rb_tree_validate(const RBTree *tree)
{
    if (!tree)
        return false;
    if (tree->root->color != RB_BLACK)
    {
        fprintf(stderr, "Invalid RBTree: Root node is not black\n");
        return false;
    }

    size_t expected = 0;
    return rb_validate_recursive(tree, tree->root, 0, &expected);
}

void rb_tree_print(const RBTree *tree)
{
    if (!tree)
    {
        printf("RBTree is NULL\n");
        return;
    }

    size_t height = rb_tree_get_height(tree);
    printf("\nRBTree Structure (height=%zu):\n", height);
    if (tree->root == tree->nil)
    {
        printf("  Empty tree\n");
        return;
    }

    for (size_t level = 1; level <= height; level++)
    {
        printf("  Level %zu: ", (unsigned long)level);
        rb_tree_print_level(tree->root, tree->nil, 1, level);
        printf("\n");
    }
    printf("----------------------------------------\n");
}

bool rb_tree_is_empty(const RBTree *tree)
{
    if (!tree)
        return true;
    return tree->root == tree->nil;
}

