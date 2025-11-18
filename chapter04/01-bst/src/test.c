#include "bst.h"

#include <stdio.h>
#include <string.h>

// -------------------------- 整数类型测试 --------------------------
int compare_int(const void *a, const void *b)
{
    int val_a = *(const int *)a;
    int val_b = *(const int *)b;
    return (val_a > val_b) - (val_a < val_b); // 简化比较逻辑
}

void visit_int(const void *data)
{
    printf("%d ", *(const int *)data);
}

void test_int_bst()
{
    printf("\n=== 整数类型BST测试 ===\n");
    BST *bst = bst_create(sizeof(int), compare_int);
    if (!bst)
    {
        fprintf(stderr, "创建BST失败\n");
        return;
    }

    // 插入测试
    int values[] = {50, 30, 70, 20, 40, 60, 80};
    printf("插入元素: ");
    for (size_t i = 0; i < sizeof(values) / sizeof(int); i++)
    {
        printf("%d ", values[i]);
        bst_insert(bst, &values[i]);
    }
    printf("\n");

    // 遍历测试
    printf("中序遍历(有序): ");
    bst_traverse(bst, IN_ORDER, visit_int);
    printf("\n");

    // 查找测试
    int key = 40;
    printf("查找 %d: %s\n", key, bst_search(bst, &key) ? "找到" : "未找到");

    // 删除测试
    printf("删除 %d 后: ", key);
    bst_remove(bst, &key);
    bst_traverse(bst, IN_ORDER, visit_int);
    printf("\n");

    // 大小测试
    printf("当前大小: %zu\n", bst_get_size(bst));

    // 清空测试
    bst_clear(bst);
    printf("清空后是否为空: %s\n", bst_is_empty(bst) ? "是" : "否");

    bst_destroy(&bst);
}

// -------------------------- 字符串类型测试 --------------------------
int compare_str(const void *a, const void *b)
{
    const char *str_a = *(const char **)a; // 注意：字符串指针的指针
    const char *str_b = *(const char **)b;
    return strcmp(str_a, str_b);
}

void visit_str(const void *data)
{
    const char *str = *(const char **)data; // 解引用字符串指针
    printf("%s ", str);
}

void test_str_bst()
{
    printf("\n=== 字符串类型BST测试 ===\n");
    BST *bst = bst_create(sizeof(char *), compare_str); // 存储字符串指针
    if (!bst)
    {
        fprintf(stderr, "创建BST失败\n");
        return;
    }

    // 插入测试（存储字符串指针）
    const char *words[] = {"apple", "banana", "cherry", "date", "elderberry"};
    printf("插入元素: ");
    for (size_t i = 0; i < sizeof(words) / sizeof(char *); i++)
    {
        printf("%s ", words[i]);
        bst_insert(bst, &words[i]); // 传入指针的地址
    }
    printf("\n");

    // 遍历测试
    printf("中序遍历(有序): ");
    bst_traverse(bst, IN_ORDER, visit_str);
    printf("\n");

    // 查找测试
    const char *key_str = "cherry";
    printf("查找 %s: %s\n", key_str,
           bst_search(bst, &key_str) ? "找到" : "未找到");

    bst_destroy(&bst);
}

// -------------------------- 结构体类型测试 --------------------------
typedef struct
{
    int id;
    char name[20];
} Student;

int compare_student(const void *a, const void *b)
{
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;
    return s1->id - s2->id; // 按ID比较
}

void visit_student(const void *data)
{
    const Student *s = (const Student *)data;
    printf("(ID: %d, Name: %s) ", s->id, s->name);
}

void test_struct_bst()
{
    printf("\n=== 结构体类型BST测试 ===\n");
    BST *bst = bst_create(sizeof(Student), compare_student);
    if (!bst)
    {
        fprintf(stderr, "创建BST失败\n");
        return;
    }

    // 插入测试
    Student students[] = {
        {3, "Bob"}, {1, "Alice"}, {4, "David"}, {2, "Charlie"}};
    printf("插入元素: ");
    for (size_t i = 0; i < sizeof(students) / sizeof(Student); i++)
    {
        printf("ID=%d ", students[i].id);
        bst_insert(bst, &students[i]);
    }
    printf("\n");

    // 遍历测试
    printf("中序遍历(有序): ");
    bst_traverse(bst, IN_ORDER, visit_student);
    printf("\n");

    // 删除测试
    Student key_stu = {2, ""}; // 只需要ID用于比较
    printf("删除 ID=2 后: ");
    bst_remove(bst, &key_stu);
    bst_traverse(bst, IN_ORDER, visit_student);
    printf("\n");

    bst_destroy(&bst);
}

int main()
{
    printf("=== 二叉搜索树多类型测试 ===\n");
    test_int_bst();    // 测试整数类型
    test_str_bst();    // 测试字符串类型
    test_struct_bst(); // 测试结构体类型
    return 0;
}

