#include "trie.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -------------------------- 测试1：结构体类型值 --------------------------
typedef struct
{
    char *name;
    int age;
} Person;

// 创建Person实例
Person *person_create(const char *name, int age)
{
    Person *p = (Person *)malloc(sizeof(Person));
    if (!p)
    {
        fprintf(stderr, "[error: person_create] 内存分配失败\n");
        return NULL;
    }

    p->name = (char *)malloc(strlen(name) + 1);
    if (!p->name)
    {
        free(p);
        return NULL;
    }
    strcpy(p->name, name);
    p->age = age;
    return p;
}

// 销毁Person实例（用于Trie的ValueDestroyer）
void person_destroy(void *value)
{
    if (!value)
        return;
    Person *p = (Person *)value;
    printf("  [销毁Person] %s (年龄: %d)\n", p->name, p->age);
    free(p->name);
    free(p);
}

// 测试结构体类型值的Trie操作
void test_struct_values(void)
{
    printf("\n=== 测试1：结构体类型值 ===\n");
    Trie *trie = trie_create(person_destroy);
    if (!trie)
    {
        fprintf(stderr, "创建Trie失败\n");
        return;
    }

    // 插入测试
    printf("1. 插入键值对...\n");
    trie_insert(trie, "apple", person_create("苹果", 5));
    trie_insert(trie, "app", person_create("应用", 10));
    trie_insert(trie, "banana", person_create("香蕉", 3));

    // 查找测试
    printf("\n2. 查找测试...\n");
    Person *p = (Person *)trie_search(trie, "app");
    if (p)
    {
        printf("查找'app'：%s (年龄: %d)\n", p->name, p->age);
    }
    else
{
        printf("查找'app'：未找到\n");
    }

    p = (Person *)trie_search(trie, "apple");
    if (p)
    {
        printf("查找'apple'：%s (年龄: %d)\n", p->name, p->age);
    }
    else
{
        printf("查找'apple'：未找到\n");
    }

    // 前缀测试
    printf("\n3. 前缀测试...\n");
    printf("前缀'app'是否存在：%s\n",
           trie_starts_with(trie, "app") ? "是" : "否");
    printf("前缀'ban'是否存在：%s\n",
           trie_starts_with(trie, "ban") ? "是" : "否");
    printf("前缀'cat'是否存在：%s\n",
           trie_starts_with(trie, "cat") ? "是" : "否");

    // 删除测试
    printf("\n4. 删除测试...\n");
    printf("删除'app'后：\n");
    trie_delete(trie, "app");
    printf("查找'app'：%s\n", trie_search(trie, "app") ? "找到" : "未找到");
    printf("前缀'app'是否存在：%s (应为是，因'apple'存在)\n",
           trie_starts_with(trie, "app") ? "是" : "否");

    // 销毁Trie
    printf("\n5. 销毁Trie（应自动销毁剩余值）...\n");
    trie_destroy(trie);
}

// -------------------------- 测试2：字符串类型值 --------------------------
// 字符串销毁函数
void string_destroy(void *value)
{
    if (!value)
        return;
    printf("  [销毁字符串] %s\n", (char *)value);
    free(value);
}

// 复制字符串（用于插入Trie时管理内存）
char *string_copy(const char *str)
{
    if (!str)
        return NULL;
    char *copy = (char *)malloc(strlen(str) + 1);
    if (copy)
        strcpy(copy, str);
    return copy;
}

// 测试字符串类型值的Trie操作
void test_string_values(void)
{
    printf("\n=== 测试2：字符串类型值 ===\n");
    Trie *trie = trie_create(string_destroy);
    if (!trie)
    {
        fprintf(stderr, "创建Trie失败\n");
        return;
    }

    // 插入测试
    printf("1. 插入键值对...\n");
    trie_insert(trie, "hello", string_copy("你好"));
    trie_insert(trie, "world", string_copy("世界"));
    trie_insert(trie, "hell", string_copy("地狱"));

    // 查找测试
    printf("\n2. 查找测试...\n");
    char *val = (char *)trie_search(trie, "hello");
    if (val)
    {
        printf("查找'hello'：%s\n", val);
    }
    else
{
        printf("查找'hello'：未找到\n");
    }

    // 删除测试
    printf("\n3. 删除测试...\n");
    printf("删除'hell'后：\n");
    trie_delete(trie, "hell");
    printf("查找'hell'：%s\n", trie_search(trie, "hell") ? "找到" : "未找到");
    printf("查找'hello'：%s (应为找到)\n",
           trie_search(trie, "hello") ? "找到" : "未找到");

    // 销毁Trie
    printf("\n4. 销毁Trie...\n");
    trie_destroy(trie);
}

// -------------------------- 测试3：整数类型值 --------------------------
// 整数无需销毁函数（直接使用NULL）
void test_int_values(void)
{
    printf("\n=== 测试3：整数类型值 ===\n");
    // 整数是简单类型，无需销毁函数
    Trie *trie = trie_create(NULL);
    if (!trie)
    {
        fprintf(stderr, "创建Trie失败\n");
        return;
    }

    // 插入测试（注意：需用动态分配的整数，避免栈内存被释放）
    printf("1. 插入键值对...\n");
    int *val1 = (int *)malloc(sizeof(int));
    *val1 = 100;
    int *val2 = (int *)malloc(sizeof(int));
    *val2 = 200;
    trie_insert(trie, "one", val1);
    trie_insert(trie, "two", val2);

    // 查找测试
    printf("\n2. 查找测试...\n");
    int *res = (int *)trie_search(trie, "two");
    if (res)
    {
        printf("查找'two'：%d\n", *res);
    }
    else
{
        printf("查找'two'：未找到\n");
    }

    // 删除测试
    printf("\n3. 删除测试...\n");
    trie_delete(trie, "one");
    printf("查找'one'：%s\n", trie_search(trie, "one") ? "找到" : "未找到");

    // 销毁Trie（因无销毁函数，需手动释放剩余整数）
    printf("\n4. 销毁Trie（手动释放剩余值）...\n");
    res = (int *)trie_search(trie, "two");
    if (res)
    {
        free(res);
    }
    trie_destroy(trie);
}

int main(void)
{
    printf("=== Trie树多类型测试 ===\n");
    test_struct_values(); // 测试结构体作为值
    test_string_values(); // 测试字符串作为值
    test_int_values();    // 测试整数作为值
    printf("\n=== 所有测试完成 ===\n");
    return 0;
}

