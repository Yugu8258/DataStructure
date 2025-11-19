#include "priority_queue.h"

#include <assert.h>
#include <stdio.h>

/**
 * @brief 测试1：基础功能（创建、入队、出队、销毁）
 */
static void test_basic_operations()
{
    printf("=== Test 1: Basic Operations ===\n");
    PriorityQueue *pq = pq_create(3); // 初始容量3
    assert(pq != NULL);
    assert(pq_size(pq) == 0);
    assert(pq_is_empty(pq) == true);

    // 入队元素（触发扩容）
    PQElement elements[] = {5, 3, 8, 1, 10, 7};
    size_t elem_len = sizeof(elements) / sizeof(elements[0]);
    for (size_t i = 0; i < elem_len; i++)
    {
        assert(pq_push(pq, elements[i]) == 0);
    }

    // 验证队列状态
    assert(pq_size(pq) == elem_len);
    assert(pq_is_empty(pq) == false);
    pq_print_debug(pq);

    // 查看队首（最高优先级元素应为10）
    PQElement top_val;
    assert(pq_top(pq, &top_val) == 0);
    assert(top_val == 10);

    // 出队（验证顺序：10→8→7→5→3→1，优先级从高到低）
    PQElement popped[] = {0, 0, 0, 0, 0, 0};
    for (size_t i = 0; i < elem_len; i++)
    {
        assert(pq_pop(pq, &popped[i]) == 0);
    }
    assert(popped[0] == 10 && popped[1] == 8 && popped[2] == 7 &&
           popped[3] == 5 && popped[4] == 3 && popped[5] == 1);

    // 空队列出队（异常测试）
    assert(pq_pop(pq, &top_val) == -1);

    // 销毁队列
    pq_destroy(&pq);
    assert(pq == NULL);

    printf("Test 1 Passed!\n\n");
}

/**
 * @brief 测试2：边界条件与异常场景
 */
static void test_boundary_and_exception()
{
    printf("=== Test 2: Boundary & Exception ===\n");

    // 测试1：创建容量为0的队列（默认容量8）
    PriorityQueue *pq1 = pq_create(0);
    assert(pq1 != NULL);
    assert(pq_size(pq1) == 0);
    pq_destroy(&pq1);

    // 测试2：空队列查看队首
    PriorityQueue *pq2 = pq_create(5);
    PQElement val;
    assert(pq_top(pq2, &val) == -1);
    pq_destroy(&pq2);

    // 测试3：插入重复元素
    PriorityQueue *pq3 = pq_create(4);
    assert(pq_push(pq3, 5) == 0);
    assert(pq_push(pq3, 5) == 0);
    assert(pq_push(pq3, 5) == 0);
    assert(pq_size(pq3) == 3);
    // 出队验证（重复元素优先级相同，顺序不保证，但均能正确取出）
    assert(pq_pop(pq3, &val) == 0 && val == 5);
    assert(pq_pop(pq3, &val) == 0 && val == 5);
    assert(pq_pop(pq3, &val) == 0 && val == 5);
    pq_print_debug(pq3);
    pq_destroy(&pq3);

    // 测试4：NULL参数防护
    assert(pq_push(NULL, 10) == -1);
    assert(pq_pop(NULL, &val) == -1);
    assert(pq_top(NULL, &val) == -1);
    assert(pq_size(NULL) == 0);
    assert(pq_is_empty(NULL) == false); // NULL队列返回false（避免误判）

    printf("Test 2 Passed!\n\n");
}

/**
 * @brief 测试3：清空队列功能
 */
static void test_pq_clear()
{
    printf("=== Test 3: PQ Clear ===\n");
    PriorityQueue *pq = pq_create(3);
    assert(pq_push(pq, 10) == 0);
    assert(pq_push(pq, 20) == 0);
    assert(pq_size(pq) == 2);

    // 清空队列
    pq_clear(pq);
    assert(pq_size(pq) == 0);
    assert(pq_is_empty(pq) == true);
    assert(pq_size(pq) == 0);

    // 清空后入队
    assert(pq_push(pq, 30) == 0);
    assert(pq_size(pq) == 1);
    PQElement top_val;
    assert(pq_top(pq, &top_val) == 0 && top_val == 30);

    pq_print_debug(pq);
    pq_destroy(&pq);
    printf("Test 3 Passed!\n\n");
}

int main()
{
    printf("===== Priority Queue Comprehensive Test =====\n\n");

    // 执行所有测试用例
    test_basic_operations();
    test_boundary_and_exception();
    test_pq_clear();

    printf("===== All Tests Passed Successfully! =====\n");
    return 0;
}

