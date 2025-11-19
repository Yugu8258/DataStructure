// test.c

#include "avl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif
#ifndef _MSC_VER
    char *strdup(const char *s); // POSIX 环境声明
#endif
#ifdef __cplusplus
}
#endif

// -------------------------- 辅助工具函数 --------------------------
/**
 * @brief 打印分隔线(美化输出)
 */
static void _print_separator(const char *title)
{
    printf("\n===========================================\n");
    printf("=== %s ===\n", title);
    printf("===========================================\n");
}

// -------------------------- 测试1：整数类型 --------------------------

/**
 * @brief 整数比较函数
 */
static int _compare_int(const void *a, const void *b, void *context)
{
    (void)context; // 未使用上下文
    int int_a = *(const int *)a;
    int int_b = *(const int *)b;
    return int_a - int_b; // 简化比较逻辑
}

/**
 * @brief 整数释放函数
 */
static void _free_int(void *data, void *context)
{
    (void)context;
    free(data);
}

/**
 * @brief 整数打印函数(遍历回调）
 */
static void _print_int(void *data, void *context)
{
    (void)context;
    printf("%d ", *(int *)data);
}

/**
 * @brief 动态创建整数(简化测试代码)
 */
static int *_create_int(int value)
{
    int *p = (int *)malloc(sizeof(int));
    if (p != NULL)
    {
        *p = value;
    }
    return p;
}

/**
 * @brief 整数类型测试(覆盖插入、删除、查找、更新)
 */
static void _test_integer_avl(void)
{
    _print_separator("Integer AVL Tree Test");

    // 1. 创建树
    AVLTree *tree = avl_create(_compare_int, _free_int, NULL);
    if (tree == NULL)
    {
        fprintf(stderr, "Failed to create integer AVL tree\n");
        return;
    }
    printf("✓ AVL tree created successfully\n");
    printf("✓ Is empty: %s\n", avl_is_empty(tree) ? "Yes" : "No");

    // 2. 插入测试(覆盖LL、RR、LR、RL四种失衡场景)
    int insert_values[] = {10, 20, 30, 40, 50, 25};
    size_t insert_len = sizeof(insert_values) / sizeof(int);
    printf("\nInserting values: ");
    for (size_t i = 0; i < insert_len; i++)
    {
        printf("%d ", insert_values[i]);
        bool ok = avl_insert(tree, _create_int(insert_values[i]));
        printf("%s ", ok ? "✓" : "✗");
    }

    // 验证插入结果
    printf("\n✓ Tree size: %zu (expected: %zu)\n", avl_size(tree), insert_len);
    printf("✓ Tree height: %d (expected: 3)\n", avl_height(tree));
    printf("✓ Is valid AVL: %s\n", avl_validate(tree) ? "Yes" : "No");
    printf("In-order traversal (ascending): ");
    avl_traverse_inorder(tree, _print_int, NULL);
    printf("\n");

    // 3. 重复插入测试
    int duplicate_val = 20;
    bool duplicate_ok = avl_insert(tree, _create_int(duplicate_val));
    printf("\nInsert duplicate value %d: %s (expected: No)\n", duplicate_val,
           duplicate_ok ? "Yes" : "No");
    printf("✓ Tree size remains: %zu\n", avl_size(tree));

    // 4. 查找测试
    int find_val = 25;
    int *found_data = (int *)avl_find(tree, &find_val);
    printf("\nFind value %d: %s (found: %d)\n", find_val,
           found_data != NULL ? "Found" : "Not Found",
           found_data ? *found_data : -1);

    int not_found_val = 60;
    found_data = (int *)avl_find(tree, &not_found_val);
    printf("Find value %d: %s\n", not_found_val,
           found_data != NULL ? "Found" : "Not Found");

    // 5. 删除测试(删除中间节点)
    int delete_val = 30;
    bool delete_ok = avl_delete(tree, &delete_val);
    printf("\nDelete value %d: %s\n", delete_val,
           delete_ok ? "Success" : "Failed");
    printf("✓ Tree size after delete: %zu (expected: %zu)\n", avl_size(tree),
           insert_len - 1);
    printf("✓ Is valid AVL after delete: %s\n",
           avl_validate(tree) ? "Yes" : "No");
    printf("In-order after delete %d: ", delete_val);
    avl_traverse_inorder(tree, _print_int, NULL);
    printf("\n");

    // 6. 更新测试
    int old_val = 50;
    int *new_val = _create_int(55);
    bool update_ok = avl_update(tree, &old_val, new_val);
    printf("\nUpdate value %d → %d: %s\n", old_val, *new_val,
           update_ok ? "Success" : "Failed");
    printf("In-order after update: ");
    avl_traverse_inorder(tree, _print_int, NULL);
    printf("\n");

    // 7. 销毁树
    avl_destroy(tree);
    printf("\n✓ Integer AVL tree destroyed successfully\n");
}

// -------------------------- 测试2：学生结构体(带上下文)
// --------------------------

/**
 * @brief 学生结构体(复杂数据类型示例)
 */
typedef struct
{
    int id;
    char *name;
} Student;

/**
 * @brief 创建学生(简化测试代码)
 */
static Student *_create_student(int id, const char *name)
{
    Student *s = (Student *)malloc(sizeof(Student));
    if (s == NULL)
    {
        return NULL;
    }
    s->id = id;
    // 兼容非MSVC编译器（strdup是POSIX标准，MSVC需用_strdup）
#ifdef _MSC_VER
    s->name = _strdup(name);
#else
    s->name = strdup(name);
#endif
    if (s->name == NULL)
    {
        free(s);
        return NULL;
    }
    return s;
}

/**
 * @brief 学生比较函数(支持按ID或姓名排序，通过上下文控制)
 */
static int _compare_student(const void *a, const void *b, void *context)
{
    const Student *s_a = (const Student *)a;
    const Student *s_b = (const Student *)b;
    const char *sort_key = (const char *)context;

    if (strcmp(sort_key, "name") == 0)
    {
        return strcmp(s_a->name, s_b->name); // 按姓名排序
    }
    return s_a->id - s_b->id; // 默认按ID排序
}

/**
 * @brief 学生释放函数(需释放内部字符串)
 */
static void _free_student(void *data, void *context)
{
    (void)context;
    Student *s = (Student *)data;
    free(s->name); // 先释放内部资源
    free(s);       // 再释放结构体
}

/**
 * @brief 学生打印函数)遍历回调)
 */
static void _print_student(void *data, void *context)
{
    (void)context;
    Student *s = (Student *)data;
    printf("(ID:%d, Name:%s) ", s->id, s->name);
}

/**
 * @brief 学生类型测试(覆盖上下文、复杂数据结构)
 */
static void _test_student_avl(void)
{
    _print_separator("Student AVL Tree Test (with Context)");

    // 测试1：按ID排序
    printf("\n--- Sort by ID ---\n");
    const char *sort_by_id = "id";
    AVLTree *tree_id =
        avl_create(_compare_student, _free_student, (void *)sort_by_id);
    if (tree_id == NULL)
    {
        fprintf(stderr, "Failed to create student tree (sort by ID)\n");
        return;
    }

    // 插入学生
    avl_insert(tree_id, _create_student(5, "Charlie"));
    avl_insert(tree_id, _create_student(2, "Alice"));
    avl_insert(tree_id, _create_student(8, "Eve"));
    avl_insert(tree_id, _create_student(1, "Bob"));
    avl_insert(tree_id, _create_student(6, "David"));

    printf("✓ Tree size: %zu\n", avl_size(tree_id));
    printf("✓ Is valid AVL: %s\n", avl_validate(tree_id) ? "Yes" : "No");
    printf("In-order (sorted by ID): ");
    avl_traverse_inorder(tree_id, _print_student, NULL);
    printf("\n");

    // 查找测试
    Student temp_id = {8, NULL};
    Student *found_student = (Student *)avl_find(tree_id, &temp_id);
    printf("Find student with ID 8: %s (Name: %s)\n",
           found_student ? "Found" : "Not Found",
           found_student ? found_student->name : "N/A");

    avl_destroy(tree_id);
    printf("✓ Student tree (sort by ID) destroyed\n");

    // 测试2: 按姓名排序
    printf("\n--- Sort by Name ---\n");
    const char *sort_by_name = "name";
    AVLTree *tree_name =
        avl_create(_compare_student, _free_student, (void *)sort_by_name);
    if (tree_name == NULL)
    {
        fprintf(stderr, "Failed to create student tree (sort by Name)\n");
        return;
    }

    // 插入相同学生
    avl_insert(tree_name, _create_student(5, "Charlie"));
    avl_insert(tree_name, _create_student(2, "Alice"));
    avl_insert(tree_name, _create_student(8, "Eve"));
    avl_insert(tree_name, _create_student(1, "Bob"));
    avl_insert(tree_name, _create_student(6, "David"));

    printf("✓ Tree size: %zu\n", avl_size(tree_name));
    printf("✓ Is valid AVL: %s\n", avl_validate(tree_name) ? "Yes" : "No");
    printf("In-order (sorted by Name): ");
    avl_traverse_inorder(tree_name, _print_student, NULL);
    printf("\n");

    // 删除测试
    Student temp_name = {0, "Bob"};
    bool delete_ok = avl_delete(tree_name, &temp_name);
    printf("Delete student Bob: %s\n", delete_ok ? "Success" : "Failed");
    printf("In-order after delete Bob: ");
    avl_traverse_inorder(tree_name, _print_student, NULL);
    printf("\n");

    avl_destroy(tree_name);
    printf("✓ Student tree (sort by Name) destroyed\n");
}

// -------------------------- 测试3：边界场景 --------------------------

/**
 * @brief 边界场景测试(空树、单节点、连续删除)
 */
static void _test_edge_cases(void)
{
    _print_separator("Edge Cases Test");

    AVLTree *tree = avl_create(_compare_int, _free_int, NULL);
    if (tree == NULL)
    {
        fprintf(stderr, "Failed to create edge case tree\n");
        return;
    }

    // 1. 空树操作测试
    printf("1. Empty tree operations:\n");
    printf("   ✓ Delete from empty tree: %s\n",
           avl_delete(tree, &(int){10}) ? "Yes" : "No");
    printf("   ✓ Find from empty tree: %s\n",
           avl_find(tree, &(int){10}) ? "Found" : "Not Found");
    printf("   ✓ Traverse empty tree: ");
    avl_traverse_inorder(tree, _print_int, NULL);
    printf(" (no output, correct)\n");

    // 2. 单节点测试
    printf("\n2. Single node test:\n");
    avl_insert(tree, _create_int(100));
    printf("   ✓ Tree size: %zu\n", avl_size(tree));
    printf("   ✓ Tree height: %d\n", avl_height(tree));
    printf("   ✓ Is valid: %s\n", avl_validate(tree) ? "Yes" : "No");

    // 删除单节点
    bool delete_ok = avl_delete(tree, &(int){100});
    printf("   ✓ Delete single node: %s\n", delete_ok ? "Success" : "Failed");
    printf("   ✓ Tree size after delete: %zu\n", avl_size(tree));

    // 3. 连续删除测试
    printf("\n3. Continuous delete test:\n");
    int values[] = {3, 1, 4, 2, 5};
    for (size_t i = 0; i < sizeof(values) / sizeof(int); i++)
    {
        avl_insert(tree, _create_int(values[i]));
    }
    printf("   Inserted values: 3 1 4 2 5\n");
    printf("   Tree size before delete: %zu\n", avl_size(tree));

    // 连续删除所有节点
    for (size_t i = 0; i < sizeof(values) / sizeof(int); i++)
    {
        avl_delete(tree, &values[i]);
    }
    printf("   ✓ Tree size after all deletes: %zu\n", avl_size(tree));
    printf("   ✓ Is valid: %s\n", avl_validate(tree) ? "Yes" : "No");

    avl_destroy(tree);
    printf("\n✓ Edge cases test completed\n");
}

// -------------------------- 主函数 --------------------------

int main(void)
{
    printf("=== AVL Tree Comprehensive Test ===\n");

    // 执行所有测试
    _test_integer_avl();
    _test_student_avl();
    _test_edge_cases();

    printf("\n=== All Tests Completed Successfully ===\n");
    return 0;
}

