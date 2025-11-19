#include "heap.h"

#include <assert.h>
#include <stdio.h>

/**
 * @brief 测试1: 基础功能 (创建、插入、提取、销毁)
 */
static void _test_basic_operations(void)
{
    printf("=== Test 1: Basic Operations ===\n");
    Heap *h = heap_create(3); // 初始容量3
    assert(h != NULL);
    assert(heap_get_capacity(h) == 3);
    assert(heap_is_empty(h) == true);

    // 插入元素（触发扩容）
    int elements[] = {5, 3, 8, 1, 10, 7};
    size_t elem_len = sizeof(elements) / sizeof(elements[0]);
    for (size_t i = 0; i < elem_len; i++)
    {
        assert(heap_insert(h, elements[i]) == 0);
        assert(heap_validate(h) == true);
    }

    // 验证堆状态
    assert(heap_get_size(h) == elem_len);
    assert(heap_validate(h) == true);
    heap_print_debug(h);

    // 查看堆顶
    Item max_val;
    assert(heap_peek_max(h, &max_val) == 0);
    assert(max_val == 10);

    // 提取最大值 (验证降序)
    Item extracted[] = {0, 0, 0, 0, 0, 0};
    for (size_t i = 0; i < elem_len; i++)
    {
        assert(heap_extract_max(h, &extracted[i]) == 0);
        if (h->size > 0)
        {
            assert(heap_validate(h) ==
                   true); // 提取后验证堆有效性 (非空堆)
        }
    }
    // 验证提取顺序 (10,8,7,5,3,1)
    assert(extracted[0] == 10 && extracted[1] == 8 && extracted[2] == 7 &&
           extracted[3] == 5 && extracted[4] == 3 && extracted[5] == 1);

    // 提取空堆 (异常测试)
    assert(heap_extract_max(h, &max_val) == -1);

    // 销毁堆
    heap_destroy(&h);
    assert(h == NULL);

    printf("Test 1 Passed!\n\n");
}

/**
 * @brief 测试2: 从数组构建堆
 */
static void _test_create_from_array(void)
{
    printf("=== Test 2: Create From Array ===\n");
    int arr[] = {4, 1, 3, 2, 16, 9, 10, 14, 8, 7};
    size_t arr_len = sizeof(arr) / sizeof(arr[0]);

    // 从数组构建堆
    Heap *h = heap_create_from_array(arr, arr_len);
    assert(h != NULL);
    assert(heap_get_size(h) == arr_len);
    assert(heap_validate(h) == true);
    heap_print_debug(h);

    // 验证堆顶为最大值16
    Item max_val;
    assert(heap_peek_max(h, &max_val) == 0);
    assert(max_val == 16);

    heap_destroy(&h);
    printf("Test 2 Passed!\n\n");
}

/**
 * @brief 测试3: 堆排序功能
 */
static void _test_heap_sort()
{
    printf("=== Test 3: Heap Sort ===\n");
    // 测试用例: 无序数组
    int arr1[] = {7, 3, 5, 2, 9, 1, 8, 4, 6};
    size_t len1 = sizeof(arr1) / sizeof(arr1[0]);
    heap_sort(arr1, len1);
    // 验证升序
    int expected1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (size_t i = 0; i < len1; i++)
    {
        assert(arr1[i] == expected1[i]);
    }
    printf("Heap Sort Test 1 (Unordered Array): ");
    for (size_t i = 0; i < len1; i++)
    {
        printf("%d ", arr1[i]);
    }
    printf("\n");

    // 测试用例: 已排序数组
    int arr2[] = {1, 2, 3, 4, 5};
    size_t len2 = sizeof(arr2) / sizeof(arr2[0]);
    heap_sort(arr2, len2);
    for (size_t i = 0; i < len2; i++)
    {
        assert(arr2[i] == i + 1);
    }
    printf("Heap Sort Test 2 (Sorted Array): ");
    for (size_t i = 0; i < len2; i++)
    {
        printf("%d ", arr2[i]);
    }
    printf("\n");

    // 测试用例: 重复元素
    int arr3[] = {5, 3, 5, 1, 5, 2};
    size_t len3 = sizeof(arr3) / sizeof(arr3[0]);
    heap_sort(arr3, len3);
    int expected3[] = {1, 2, 3, 5, 5, 5};
    for (size_t i = 0; i < len3; i++)
    {
        assert(arr3[i] == expected3[i]);
    }
    printf("Heap Sort Test 3 (Duplicate Elements): ");
    for (size_t i = 0; i < len3; i++)
    {
        printf("%d ", arr3[i]);
    }
    printf("\n");

    printf("Test 3 Passed!\n\n");
}

/**
 * @brief 测试4: 边界条件与异常场景
 */
static void _test_boundary_and_exception(void)
{
    printf("=== Test 4: Boundary & Exception ===\n");

    // 测试1: 创建容量为0的堆 (默认容量8)
    Heap *h1 = heap_create(0);
    assert(h1 != NULL);
    assert(heap_get_capacity(h1) == 8);
    heap_destroy(&h1);

    // 测试2: 空堆操作
    Heap *h2 = heap_create(5);
    Item val;
    assert(heap_peek_max(h2, &val) == -1);
    assert(heap_extract_max(h2, &val) == -1);
    heap_clear(h2); // 清空空堆（无异常）
    heap_destroy(&h2);

    // 测试3: 插入重复元素
    Heap *h3 = heap_create(4);
    assert(heap_insert(h3, 5) == 0);
    assert(heap_insert(h3, 5) == 0);
    assert(heap_insert(h3, 5) == 0);
    assert(heap_get_size(h3) == 3);
    assert(heap_validate(h3) == true);
    heap_print_debug(h3);
    heap_destroy(&h3);

    // 测试4: NULL参数防护
    assert(heap_create_from_array(NULL, 5) == NULL);
    assert(heap_create_from_array((int[]){1,2,3}, 0) == NULL); // 用临时数组替代未定义变量
    assert(heap_insert(NULL, 10) == -1);
    assert(heap_peek_max(NULL, &val) == -1);
    assert(heap_extract_max(NULL, &val) == -1);
    assert(heap_get_size(NULL) == 0);

    printf("Test 4 Passed!\n\n");
}

/**
 * @brief 测试5: 清空堆功能
 */
static void _test_heap_clear(void)
{
    printf("=== Test 5: Heap Clear ===\n");
    int arr[] = {10, 20, 30};
    Heap *h = heap_create_from_array(arr, sizeof(arr) / sizeof(arr[0]));
    assert(heap_get_size(h) == 3);

    // 清空堆
    heap_clear(h);
    assert(heap_get_size(h) == 0);
    assert(heap_is_empty(h) == true);
    assert(heap_get_capacity(h) == 3); // 容量不变

    // 清空后插入元素
    assert(heap_insert(h, 40) == 0);
    assert(heap_get_size(h) == 1);
    assert(heap_validate(h) == true);

    heap_print_debug(h);
    heap_destroy(&h);
    printf("Test 5 Passed!\n\n");
}

int main(void)
{
    printf("===== Max Heap Comprehensive Test =====\n\n");

    // 执行所有测试用例
    _test_basic_operations();
    _test_create_from_array();
    _test_heap_sort();
    _test_boundary_and_exception();
    _test_heap_clear();

    printf("===== All Tests Passed Successfully! =====\n");
    return 0;
}

