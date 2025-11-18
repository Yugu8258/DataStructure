// safe_array.c

#include "safe_array.h"

#include <stdio.h>
#include <stdlib.h>

// --- Private Structure Definition ---
// "安全数组"的内部秘密: 它其实是一个结构体,
// 封装了真正的原始数组指针和它的容量.
struct SafeArray
{
    int *data;       // 指向动态分配的原始整数数组
    size_t capacity; // 数组的容量
};

// ===================================================================
// ==================== 私有辅助函数 - "部门经理" ====================
// ===================================================================
// 这些函数被声明为 static, 意味着它们只能在本文件(.c)内部被调用.
// 它们是"内部员工", 用户(其它.c文件)永远无法直接接触到它们.
// 它们假设所有检查工作已经被上级做完了, 只专注于自己的核心任务.

/**
 * @brief [私有] 真正执行设置值的操作
 * @note 这个函数是"不安全"的, 因为它假设索引是有效的, 合法的.
 *       它的存在, 是为了把"核心操作"和"安全检查"分离开来.
 *
 * @param sarray [in] 指向"安全数组"的指针
 * @param index [in] 要设置的值的索引
 * @param value [in] 要设置的值
 */
static void _set_value(SafeArray *sarray, size_t index, int value)
{
    // 部门经理的核心工作: 就这一行代码!
    // 他相信他的CEO上级已经检查过index的合法性了.
    sarray->data[index] = value;
}

/**
 * @brief [私有] 真正执行获取值的操作
 *
 * @param sarray [in] 指向"安全数组"的指针
 * @param index [in] 要获取的值的索引
 */
static int _get_value(const SafeArray *sarray, size_t index)
{
    // 部门经理的核心工作: 就这一行代码!
    return sarray->data[index];
}

// ======================================================================
// ====================  公共API函数 - “公司CEO”  ======================
// ======================================================================

SafeArray *sarray_create(size_t capacity)
{
    // CEO的职责1：检查初始参数的合法性
    if (capacity == 0)
        return NULL;

    SafeArray *sa = (SafeArray *)malloc(sizeof(SafeArray));
    if (!sa)
        return NULL;

    // CEO的职责2：负责资源的分配
    sa->data = (int *)malloc(capacity * sizeof(int));
    if (!sa->data)
    {
        free(sa);
        return NULL;
    }

    sa->capacity = capacity;
    return sa;
}

void sarray_destroy(SafeArray **p_sarray)
{
    // CEO的职责：负责资源的释放
    if (p_sarray && *p_sarray)
    {
        free((*p_sarray)->data); // 先释放内部的数组
        free(*p_sarray);         // 再释放结构体本身
        *p_sarray = NULL;
    }
}

bool sarray_set(SafeArray *sarray, size_t index, int value)
{
    /*
     * 函数功能: [CEO] 安全地设置一个值。
     * ------------------------- 教学案例 -------------------------
     * 假设我们调用 sarray_set(my_array, 2, 99)。
     * 1. CEO (sarray_set) 接到请求。
     * 2. CEO 做的第一件事，就是进行严格的“安检”。
     *    - 检查 my_array 是否存在？(sarray != NULL)
     *    - 检查索引 2 是否在合法范围内 [0, capacity-1]？
     * 3. 如果安检通过，CEO 就把“将99这个值放到2号位置”这个具体的任务，
     *    委托给内部的“部门经理” `_set_value` 去执行。
     * 4. CEO 不关心经理具体是怎么写的，他只知道任务会完成。
     * 5. CEO 向客户报告：任务成功 (return true)。
     * -----------------------------------------------------------
     */

    // 步骤 1: CEO进行严格的“安检”。这是公共API的核心职责。
    // 检查数组指针是否有效，以及索引是否越界。
    if (!sarray || index >= sarray->capacity)
    {
        // 如果安检不通过，直接拒绝服务，报告失败。
        return false;
    }

    // 步骤 2: 安检通过后，将核心任务委托给“部门经理”。

    // 步骤3： 任务委托。这里体现了职责分离：CEO负责安全，经理负责执行。
    _set_value(sarray, index, value);

    //    sarray->data[index] = value;

    // 职责分离
    // Separation of Concerns

    // 步骤 3: 报告任务成功。
    return true;
}

bool sarray_get(const SafeArray *sarray, size_t index, int *out_value)
{
    /*
     * 函数功能: [CEO] 安全地获取一个值。
     * 逻辑与 sarray_set 完全相同，只是委托给了不同的“部门经理”。
     */

    // 步骤 1: CEO进行严格的“安检”。
    // 检查数组指针、索引、以及用于接收结果的输出指针是否都有效。
    if (!sarray || index >= sarray->capacity || !out_value)
    {
        return false;
    }

    // 步骤 2: 安检通过，委托给“部门经理”`_get_value`去取回数据。
    int value = _get_value(sarray, index);

    // 步骤 3: 将经理取回的数据，通过安全的“输出参数”方式交给客户。
    *out_value = value;

    // void *
    // memcpy(out_value, &value, sizeof(int));

    // 步骤 4: 报告任务成功。
    return true;
}

size_t sarray_get_capacity(const SafeArray *sarray)
{
    if (!sarray)
        return 0;
    return sarray->capacity;
}

void sarray_print(const SafeArray *sarray)
{
    if (!sarray)
    {
        printf("SafeArray is NULL.\n");
        return;
    }
    printf("SafeArray (capacity: %zu) [ ", sarray->capacity);
    for (size_t i = 0; i < sarray->capacity; i++)
    {
        // 为了打印，我们在这里“作弊”直接访问了内部数据。
        // 在实际应用中，我们甚至可以不提供打印函数，以保证绝对的封装。
        printf("%d ", sarray->data[i]);
    }
    printf("]\n");
}

