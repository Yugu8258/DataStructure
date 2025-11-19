#include "red_black_tree.h"

#include <assert.h>
#include <stdio.h>

static void print_key(RBData key)
{
    printf("%d ", key);
}

static void test_insert(void)
{
    printf("=== Test 1: Insert Operation ===\n");
    RBTree *tree = rb_tree_create();
    assert(tree != NULL);

    int keys[] = {10, 20, 30, 15, 25, 5, 35, 20, -5, 0};
    size_t key_len = sizeof(keys) / sizeof(keys[0]);

    for (size_t i = 0; i < key_len; i++)
    {
        int ret = rb_tree_insert(tree, keys[i]);
        assert(ret == 0);
    }

    assert(rb_tree_validate(tree) == true);

    printf("Inorder traversal (sorted): ");
    rb_tree_inorder_traversal(tree, print_key);
    printf("\n");

    rb_tree_print(tree);

    rb_tree_destroy(tree);
    printf("Test 1 Passed!\n\n");
}

static void test_search(void)
{
    printf("=== Test 2: Search Operation ===\n");
    RBTree *tree = rb_tree_create();
    assert(tree != NULL);

    int keys[] = {5, 3, 7, 2, 4, 6, 8};
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
    {
        rb_tree_insert(tree, keys[i]);
    }

    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
    {
        RBNode *node = rb_tree_search(tree, keys[i]);
        assert(node != NULL);
        assert(node->key == keys[i]);
        printf("Found key: %d (color: %s)\n", node->key,
               (node->color == RB_RED) ? "RED" : "BLACK");
    }

    assert(rb_tree_search(tree, 1) == NULL);
    assert(rb_tree_search(tree, 9) == NULL);
    assert(rb_tree_search(tree, -1) == NULL);

    rb_tree_destroy(tree);
    printf("Test 2 Passed!\n\n");
}

static void test_delete(void)
{
    printf("=== Test 3: Delete Operation ===\n");
    RBTree *tree = rb_tree_create();
    assert(tree != NULL);

    int keys[] = {10, 20, 30, 15, 25, 5, 35, -5, 0};
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
    {
        rb_tree_insert(tree, keys[i]);
    }

    printf("Tree before deletion:\n");
    rb_tree_print(tree);

    assert(rb_tree_delete(tree, -5) == 0);
    assert(rb_tree_validate(tree) == true);

    assert(rb_tree_delete(tree, 0) == 0);
    assert(rb_tree_validate(tree) == true);

    assert(rb_tree_delete(tree, 20) == 0);
    assert(rb_tree_validate(tree) == true);

    assert(rb_tree_delete(tree, 10) == 0);
    assert(rb_tree_validate(tree) == true);

    printf("Tree after deletion (deleted -5, 0, 20, 10):\n");
    rb_tree_print(tree);

    printf("Inorder traversal after deletion: ");
    rb_tree_inorder_traversal(tree, print_key);
    printf("\n");

    assert(rb_tree_delete(tree, 100) == -1);

    rb_tree_destroy(tree);
    printf("Test 3 Passed!\n\n");
}

static void test_boundary(void)
{
    printf("=== Test 4: Boundary & Exception ===\n");

    RBTree *tree = rb_tree_create();
    assert(tree != NULL);
    assert(rb_tree_is_empty(tree) == true);
    assert(rb_tree_delete(tree, 10) == -1);
    assert(rb_tree_search(tree, 10) == NULL);
    assert(rb_tree_get_height(tree) == 0);

    assert(rb_tree_insert(tree, 5) == 0);
    assert(tree->root->color == RB_BLACK);
    assert(rb_tree_validate(tree) == true);

    assert(rb_tree_insert(tree, 3) == 0);
    assert(tree->root->left->color == RB_RED);
    assert(rb_tree_validate(tree) == true);

    assert(rb_tree_insert(NULL, 10) == -1);
    assert(rb_tree_delete(NULL, 10) == -1);
    assert(rb_tree_search(NULL, 10) == NULL);
    assert(rb_tree_get_height(NULL) == 0);

    rb_tree_destroy(tree);
    printf("Test 4 Passed!\n\n");
}

int main(void)
{
    printf("===== Red-Black Tree Comprehensive Test =====\n\n");
    test_insert();
    test_search();
    test_delete();
    test_boundary();

    printf("===== All Tests Passed Successfully! =====\n");
    return 0;
}

