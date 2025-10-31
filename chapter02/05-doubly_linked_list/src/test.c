#include "doubly_linked_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 員工數據結構(測試用例)
 */
typedef struct employee
{
    int id;     // 員工ID (唯一標識)
    char *name; // 員工姓名 (動態分配)
} employee;

/**
 * @brief 字符串複製工具函數 (跨平臺兼容)
 * @param str 源字符串
 * @return 複製後的新字符串 (需手動free), 失敗返回NULL
 */
static char *_portable_strdup(const char *str)
{
    if (!str)
    {
        return NULL;
    }

    size_t len = strlen(str);
    char *new_str = (char *)malloc(len + 1); // +1 存儲結束符 '\0'
    if (new_str)
    {
        strcpy(new_str, str);
    }

    return new_str;
}

/**
 * @brief 員工數據釋放函數 (適配free_func)
 * @param data 員工指針 (需轉換爲employee *)
 */
static void _free_employee(void *data)
{
    employee *emp = (employee *)data;
    if (emp)
    {
        printf("[_free_employee] 釋放員工: ID = %d, 姓名 = %s\n", emp->id,
               emp->name);
        free(emp->name); // 先釋放姓名
        free(emp);       // 再釋放員工結構
    }
}

/**
 * @brief 員工ID比較函數 (適配compare_func)
 * @param a 員工a指針
 * @param b 員工b指針
 * @return 0表示ID相等, 正數表示a的ID更大, 負數反之
 */
static int _compare_employee_by_id(const void *a, const void *b)
{
    const employee *emp_a = (const employee *)a;
    const employee *emp_b = (const employee *)b;
    return emp_a->id - emp_b->id;
}

/**
 * @brief 員工打印操作函數 (適配action_func)
 * @param data 員工指針
 * @param context 上下文參數 (此處未使用)
 */
static void _print_employee_action(void *data, void *context)
{
    const employee *emp = (const employee *)data;
    if (!emp)
    {
        printf("無效員工數據\n");
        return;
    }

    printf("員工信息: Id = %d, 姓名 = %s\n", emp->id, emp->name);
}

int main(int argc, char *argv[])
{
    printf("==== 雙向鏈表的ADT實現-內存池與多線程互斥鎖 ====\n");

    // 1. 創建鏈表 (內存池容量爲0, 指定員工釋放函數)
    printf("[步驟1] 創建鏈表 (內存池融倆爲10)...\n");
    int capacity = 10;

    doubly_linked_list *emp_list = list_create(capacity, _free_employee);
    if (!emp_list)
    {
        fprintf(stderr, "鏈表創建失敗, 退出程序\n");
        return EXIT_FAILURE;
    }

    printf("鏈表創建成功, 初始化大小: %zu\n\n", list_get_size(emp_list));

    // 2. 尾部追加5名員工
    printf("[步驟2] 追加五名員工...\n");
    for (size_t i = 0; i < 5; ++i)
    {
        employee *emp = (employee *)malloc(sizeof(*emp));
        if (!emp)
        {
            fprintf(stderr, "員工內存分配失敗, 跳過!\n");
            continue;
        }

        emp->id = 101 + i; // ID從101開始

        char buffer[50];

        sprintf(buffer, "員工%d\n", emp->id);

        emp->name = _portable_strdup(buffer);
        if (!emp->name)
        {
            fprintf(stderr, "姓名內存分配失敗, 跳過\n");
            free(emp);
            continue;
        }

        list_append(emp_list, emp); // 追加到鏈表
    }

    // 3. 打印當前員工鏈表
    printf("\n[步驟3] 打印員工列表...\n");

    list_for_each(emp_list, _print_employee_action, NULL);

    printf("當前員工數量: %zu\n\n", list_get_size(emp_list));

    // 4. 查找ID = 103的員工
    printf("\n[步驟4] 查找Id = 103的員工...\n");
    employee search_key = {103, NULL}; // 僅需Id作爲關鍵字
    doubly_list_node *found_node =
        list_find(emp_list, &search_key, _compare_employee_by_id);
    if (found_node)
    {
        employee *found_emp = (employee *)list_get_data(found_node);
        printf("找到員工: Id = %d, 姓名 = %s\n", found_emp->id,
               found_emp->name);
    }
    else
    {
        printf("沒有找到 Id = 103 員工\n");
    }

    // 5. 刪除 Id = 103 的員工
    if (found_node)
    {
        printf("\n[步驟5]刪除Id = 103 的員工...\n");
        list_delete_node(emp_list, found_node);
        printf("刪除後員工數量: %zu\n", list_get_size(emp_list));
        printf("刪除後列表: \n");
        list_for_each(emp_list, _print_employee_action, NULL);
    }

    // 6. 頭部插入CEO
    printf("\n[步驟6] 在頭部插入CEO...\n");
    employee *ceo = (employee *)malloc(sizeof(*ceo));
    if (ceo)
    {
        ceo->id = 99;
        ceo->name = _portable_strdup("CEO");
        if (ceo->name)
        {
            list_prepend(emp_list, ceo);
            printf("插入後列表: \n");
            list_for_each(emp_list, _print_employee_action, NULL);
            printf("當前員工數量: %zu\n", list_get_size(emp_list));
        }
        else
        {
            fprintf(stderr, "CEO姓名分配失敗\n");
            free(ceo);
        }
    }
    else
    {
        fprintf(stderr, "CEO內存分配失敗\n");
    }

    // 7. 銷燬鏈表 (釋放所有資源)
    printf("\n[步驟7] 銷燬鏈表...\n");
    list_destroy(&emp_list);

    printf("鏈表銷燬後指針狀態: %s\n",
           (emp_list == NULL) ? "已置空" : "爲置空");

    puts("\n===== 所有測試完成 ====\n");

    return EXIT_SUCCESS;
}

