#include "node.h"

#include <stdio.h>
#include <stdlib.h>

// --------------------------
// 回呼函數聲明（測試專用）
// --------------------------
// 1. 列印學生資料
void print_student(const void *data);

// 2. 僅按學號比較（無上下文）
int compare_by_id(const void *a, const void *b, void *context);

// 3. 按學號+最小年齡比較（需上下文）
// 上下文結構體：存儲最小年齡要求
typedef struct search_context
{
    int min_age_required;
} search_context;
int compare_by_id_and_min_age(const void *a, const void *b, void *context);

// 4. 學生資料釋放回呼（當前無動態記憶體，僅作演示）
void free_student_data(void *data);

// --------------------------
// 主測試函數
// --------------------------
int main(void)
{
    puts("\n==== 單向鏈表的學生類別實現 ====\n");

    node *head = NULL; // 鏈表表頭初始化（空鏈表）

    // 1. 初始化鏈表：尾部追加4個學生
    printf("--- 1. 初始化鏈表（尾部追加學生） ---\n");
    student students[] = {{101, "Alice", 22},
                          {102, "Bob", 19},
                          {103, "Carol", 25},
                          {104, "David", 19}};
    size_t student_count = sizeof(students) / sizeof(students[0]);
    for (size_t i = 0; i < student_count; ++i)
    {
        append_node(&head, students[i]);
    }
    print_list(head, print_student);

    // 2. 刪除學號103的學生（Carol）
    printf("\n--- 2. 刪除學號為103的學生（Carol） ---\n");
    student target_carol = {103, "", 0};                    // 僅需學號用於比較
    delete_node(&head, &target_carol, compare_by_id, NULL); // 無上下文
    print_list(head, print_student);

    // 3. 查找「學號104 + 年齡≥20」的學生（David年齡19，預期未找到）
    printf("\n--- 3. 查找學號104且年齡≥20的學生 ---\n");
    search_context ctx_fail = {20}; // 上下文：最小年齡20
    student target_david = {104, "", 0};
    node *found =
        find_node(head, &target_david, compare_by_id_and_min_age, &ctx_fail);
    if (found)
    {
        printf("找到了: ");
        print_student(&found->data);
        printf("\n");
    }
    else
    {
        printf("沒有找到（因為學號104的David年齡僅19歲）\n");
    }

    // 4. 查找「學號101 + 年齡≥20」的學生並更新（Alice符合，預期更新成功）
    printf("\n--- 4. 查找學號101且年齡≥20的學生並更新 ---\n");
    search_context ctx_success = {20}; // 上下文：最小年齡20
    student target_alice = {101, "", 0};
    student new_alice_data = {101, "Alicia",
                              23}; // 新資料：姓名改為Alicia，年齡23
    update_node(&head, &target_alice, new_alice_data, compare_by_id_and_min_age,
                &ctx_success);
    print_list(head, print_student);

    // 5. 釋放鏈表記憶體（避免記憶體洩漏）
    printf("\n--- 5. 釋放所有記憶體 ---\n");
    free_list(&head, free_student_data); // 傳入資料釋放回呼（此處無實際操作）
    printf("鏈表已經清空！\n");
    print_list(head, print_student); // 空鏈表，輸出NULL

    return EXIT_SUCCESS;
}

// --------------------------
// 回呼函數實現
// --------------------------
void print_student(const void *data)
{
    const student *stu = (const student *)data;
    printf("{學號: %d, 姓名: %s, 年齡: %d}", stu->id, stu->name, stu->age);
}

int compare_by_id(const void *a, const void *b, void *context)
{
    (void)context; // 顯式忽略未使用的參數，消除編譯器警告

    const student *stu_a = (const student *)a;
    const student *target_stu = (const student *)b;

    // 學號相等返回0（匹配），否則返回1（不匹配）
    return (stu_a->id == target_stu->id) ? 0 : 1;
}

int compare_by_id_and_min_age(const void *a, const void *b, void *context)
{
    const student *stu_a = (const student *)a;
    const student *target_stu = (const student *)b;
    search_context *ctx = (search_context *)context;

    // 檢查上下文是否有效
    if (!ctx)
    {
        fprintf(
            stderr,
            "警告: compare_by_id_and_min_age - 上下文為空，使用預設年齡0\n");
        return (stu_a->id == target_stu->id) ? 0 : 1;
    }

    // 同時滿足「學號相等」和「年齡≥最小要求」才返回0（匹配）
    return (stu_a->id == target_stu->id && stu_a->age >= ctx->min_age_required)
               ? 0
               : 1;
}

void free_student_data(void *data)
{
    // 若student結構體中的name是動態分配的char*，需在此處釋放：
    // student* stu = (student*)data;
    // free(stu->name);  // 釋放動態記憶體
    // 當前範例使用陣列，無需釋放
}

