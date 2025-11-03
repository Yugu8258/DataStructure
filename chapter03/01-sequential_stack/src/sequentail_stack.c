#include "sequentail_satck.h"

#include <assert.h> // 用於調試時的參數校驗
#include <stdlib.h> // 用於malloc、free
#include <string.h> // 用於memcpy

/**
 * @file sequentail_satck.c
 * @brief 順序棧的具體實現
 * @details 內部使用動態數組存儲元素, 通過棧頂指針 (top) 管理元素進出.
 * 所有操作的時間複雜度爲O(1).
 */

// 棧的內部結構 (不透明實現, 用戶不可見)
struct Stack
{
    void *data;          // 存儲元素的連續內存塊 (動態數組)
    size_t capacity;     // 棧的最大容量 (元素數量)
    size_t element_size; // 單個元素的字節大小
    int top;             // 棧頂索引: -1表示空棧, 最大值爲capacity - 1
};

/**
 * @brief 計算棧中指定索引位置的元素地址
 * @param stack 棧指針
 * @param index 元素索引 (0-based)
 * @return 該索引對應的元素地址 (char *類型便於字節級操作)
 * @note 內部輔助函數, 僅在實現中使用
 */
static char *_get_element_address(const Stack *stack, int index)
{
    // 轉換爲 char * 以支持字節級偏移計算 (void *不支持指針計算)
    return (char *)stack->data + (index * stack->element_size);
}

Stack *stack_create(size_t capacity, size_t element_size)
{
    // 參數校驗: 容量和元素大小必須爲正數
    if (capacity == 0 || element_size == 0)
    {
        return NULL;
    }

    // 1. 配置棧結構體內存
    Stack *stack = (Stack *)malloc(sizeof(*stack));
    if (!stack)
    {
        return NULL; // 內存分配失敗
    }

    // 2. 分配元素存儲區內存 (連續塊)
    stack->data = (void *)malloc(capacity * element_size);
    if (!stack->data)
    {
        free(stack); // 回滾已分配的結構體內存
        return NULL;
    }

    // 3. 初始化棧屬性
    stack->capacity = capacity;
    stack->element_size = element_size;
    stack->top = -1; // 初始棧爲空

        return stack;
}

void stack_destroy(Stack **p_stack)
{
    // 避免對空指針操作
    if (!p_stack || !*p_stack)
    {
        return;
    }

    // 1. 釋放元素存儲區
    free((*p_stack)->data);
    // 2. 釋放棧結構體
    free(*p_stack);
    // 3. 置空外部指針, 防止懸掛
    *p_stack = NULL;
}

bool stack_push(Stack *stack, const void *element_data)
{
    // 參數校驗 (調試模式下觸發斷言, release 模式下返回 false)
    assert(stack && "stack_push: stack is NULL");
    assert(element_data && "stack_push: element_data is NULL");

    // 檢查參數有效性和棧是否滿
    if (!stack || !element_data || stack_is_full(stack))
    {
        return false;
    }

    // 移動棧頂指針 (指向新元素位置)
    stack->top++;

    // 計算新元素的存儲地址
    char *target_addr = _get_element_address(stack, stack->top);

    // 複製元素數據到棧內 (深拷貝, 避免外部數據釋放影響棧)
    memcpy(target_addr, element_data, stack->element_size);

    return true;
}

void stack_pop(Stack *stack, void *output_buffer)
{
    // 參數校驗(調試模式下觸發斷言)
    assert(stack && "stack_pop: output_buffer is NULL");
    assert(output_buffer && "stack_pop: output_buffer is NULL");
    assert(!stack_is_empty(stack) && "stack_pop: stack is empty");

    // 計算棧頂元素地址
    char *source_addr = _get_element_address(stack, stack->top);

    // 複製元素數據到輸出緩衝區
    memcpy(output_buffer, source_addr, stack->element_size);

    // 移動棧頂指針 (邏輯刪除元素)
    stack->top--;
}

bool stack_peek(const Stack *stack, void *output_buffer)
{
    // 參數校驗
    assert(stack && "stack_peek: stack is NULL");
    assert(output_buffer && "stack_peek: output_buffer is NULL");

    // 檢查參數有效性和棧是否爲空
    if (!stack || !output_buffer || stack_is_empty(stack) )
    {
        return false;
    }

    // 計算棧頂元素地址並複製數據 (不修改棧頂指針)
    char *source_addr = _get_element_address(stack, stack->top);
    memcpy(output_buffer, source_addr, stack->element_size);

    return true;
}

bool stack_is_empty(const Stack *stack)
{
    // 空指針視爲空棧
    return (!stack) ? 0 : (stack->top == -1);
}

bool stack_is_full(const Stack *stack)
{
    // 空指針視爲爲滿
    // 棧頂索引到達最大容量-1時未滿 (注意類型轉換避免符號警告)
    return (!stack) ? false : (stack->top == (int)(stack->capacity - 1));
}

size_t stack_get_size(const Stack *stack)
{
    // 元素數量 = 棧頂索引 + 1 (空棧時top = -1. 返回0)
    return (!stack) ? 0 : ((size_t)(stack->top + 1));
}

size_t stack_get_capacity(const Stack *stack)
{
    return (stack) ? stack->capacity : 0;
}

