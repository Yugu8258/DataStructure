#include "circular_linked_list.h"

#include <stdio.h>
#include <stdlib.h>

// --------------------------
// 回呼函數宣告（自定義業務邏輯）
// --------------------------
// 1. 列印學生資料（格式化輸出）
void print_student(const void *data);

// 2. 僅按「學號」比較（無需額外參數，context 未使用）
int compare_by_id(const void *a, const void *b, void *context);

// 3. 按「學號 + 最小年齡」比較（需 context 傳遞最小年齡要求）
typedef struct search_context
{
    int min_age; // 查詢的最小年齡門檻
} search_context;
int compare_by_id_and_age(const void *a, const void *b, void *context);

// 4. 學生資料釋放回呼（若有動態記憶體需在此釋放）
void free_student(void *data);

// --------------------------
// 主測試函數（全流程測試）
// --------------------------
int main(void)
{
    puts("==== 單向循環鏈表（學生資料示範） ====\n");

    node *head = NULL; // 表頭初始化（空鏈表）

    // 1. 尾部追加 3 個學生（101-Alice、102-Bob、103-Carol）
    printf("--- 1. 尾部追加學生 ---\n");
    student stu1 = {101, "Alice", 22};
    student stu2 = {102, "Bob", 19};
    student stu3 = {103, "Carol", 25};
    append_node(&head, stu1);
    append_node(&head, stu2);
    append_node(&head, stu3);
    print_list(head, print_student); // 列印此時的循環鏈表

    // 2. 头部插入 1 個學生（100-David，成為新表頭）
    printf("\n--- 2. 头部插入學生（100-David） ---\n");
    student stu4 = {100, "David", 21};
    prepend_node(&head, stu4);
    print_list(head, print_student); // 驗證新表頭是否生效

    // 3. 場景1：查詢「學號=102」的學生（Bob，預期找到）
    printf("\n--- 3. 查詢：學號=102 的學生 ---\n");
    student target_id = {102, "", 0}; // 僅需學號用於比較，其他欄位無效
    node *found_id = find_node(head, &target_id, compare_by_id, NULL);
    if (found_id)
    {
        printf("✅ 找到目標學生：");
        print_student(&found_id->data);
        printf("\n");
    }
    else
    {
        printf("❌ 未找到學號=102 的學生\n");
    }

    // 4. 場景2：查詢「學號=101 且年齡≥22」的學生（Alice，年齡剛好22，預期找到）
    printf("\n--- 4. 查詢：學號=101 且年齡≥22 的學生 ---\n");
    search_context ctx_success = {.min_age = 22}; // 設定最小年齡門檻
    student target_id_age1 = {101, "", 0};
    node *found_id_age1 =
        find_node(head, &target_id_age1, compare_by_id_and_age, &ctx_success);
    if (found_id_age1)
    {
        printf("✅ 找到目標學生：");
        print_student(&found_id_age1->data);
        printf("\n");
    }
    else
    {
        printf("❌ 未找到學號=101 且年齡≥22 的學生\n");
    }

    // 5. 場景3：查詢「學號=102 且年齡≥20」的學生（Bob年齡19，預期未找到）
    printf("\n--- 5. 查詢：學號=102 且年齡≥20 的學生 ---\n");
    search_context ctx_fail = {.min_age = 20};
    student target_id_age2 = {102, "", 0};
    node *found_id_age2 =
        find_node(head, &target_id_age2, compare_by_id_and_age, &ctx_fail);
    if (found_id_age2)
    {
        printf("✅ 找到目標學生：");
        print_student(&found_id_age2->data);
        printf("\n");
    }
    else
    {
        printf("❌ 未找到學號=102 且年齡≥20 的學生（Bob年齡僅19）\n");
    }

    // 6. 場景1：刪除「非表頭節點」（學號=103 的 Carol，預期成功）
    printf("\n--- 6. 刪除：學號=103 的學生（非表頭） ---\n");
    student target_del1 = {103, "", 0};
    delete_node(&head, &target_del1, compare_by_id, NULL);
    print_list(head, print_student); // 驗證 Carol 是否被刪除

    // 7. 場景2：刪除「表頭節點」（學號=100 的 David，預期成功）
    printf("\n--- 7. 刪除：學號=100 的學生（表頭） ---\n");
    student target_del2 = {100, "", 0};
    delete_node(&head, &target_del2, compare_by_id, NULL);
    print_list(head, print_student); // 驗證新表頭是否切換為 Alice

    // 8. 場景3：刪除「最後一個節點」（先刪除 Bob，使 Alice
    // 成為唯一節點，再刪除）
    printf("\n--- 8. 刪除：最後一個節點（模擬空鏈表） ---\n");
    // 步驟8.1：先刪除 Bob，此時鏈表只剩 Alice
    student target_del3 = {102, "", 0};
    delete_node(&head, &target_del3, compare_by_id, NULL);
    printf("🔹 刪除 Bob 後的鏈表（只剩 Alice）：\n");
    print_list(head, print_student);

    // 步驟8.2：刪除唯一節點 Alice，鏈表變空
    student target_del4 = {101, "", 0};
    delete_node(&head, &target_del4, compare_by_id, NULL);
    printf("🔹 刪除 Alice 後的鏈表：\n");
    print_list(head, print_student); // 驗證空鏈表輸出

    // 9. 釋放鏈表記憶體（即使是空鏈表，調用也安全）
    printf("\n--- 9. 釋放循環鏈表所有記憶體 ---\n");
    free_list(&head, free_student);
    print_list(head, print_student); // 再次驗證空鏈表

    puts("\n==== 所有測試場景執行完畢 ====");
    return EXIT_SUCCESS;
}

// --------------------------
// 回呼函數具體實現
// --------------------------

/**
 * 列印學生資料的回呼函數
 * @param data 指向 student 結構體的 void 指標（需強轉）
 */
void print_student(const void *data)
{
    const student *stu = (const student *)data;
    // 格式化輸出：學號、姓名、年齡
    printf("{學號: %d, 姓名: %s, 年齡: %d}", stu->id, stu->name, stu->age);
}

/**
 * 僅按學號比較的回呼函數
 * @param a 指向當前節點 data 的指標
 * @param b 指向目標 data 的指標
 * @param context 未使用（顯式忽略以消除編譯警告）
 * @return 0：學號相等；1：學號不相等
 */
int compare_by_id(const void *a, const void *b, void *context)
{
    (void)context; // 顯式忽略未使用的參數

    const student *stu_a = (const student *)a;
    const student *stu_target = (const student *)b;

    return (stu_a->id == stu_target->id) ? 0 : 1;
}

/**
 * 按「學號 + 最小年齡」比較的回呼函數
 * @param a 指向當前節點 data 的指標
 * @param b 指向目標 data 的指標
 * @param context 指向 search_context 的指標（存儲最小年齡要求）
 * @return 0：學號相等且年齡≥最小要求；1：不滿足條件
 */
int compare_by_id_and_age(const void *a, const void *b, void *context)
{
    const student *stu_a = (const student *)a;
    const student *stu_target = (const student *)b;
    search_context *ctx = (search_context *)context;

    // 防禦性判斷：若 context 為空，預設最小年齡為 0
    if (!ctx)
    {
        fprintf(stderr,
                "⚠️ compare_by_id_and_age：context 為空，預設最小年齡=0\n");
        return (stu_a->id == stu_target->id) ? 0 : 1;
    }

    // 雙條件判斷：學號相等 + 年齡≥最小要求
    return (stu_a->id == stu_target->id && stu_a->age >= ctx->min_age) ? 0 : 1;
}

/**
 * 學生資料釋放的回呼函數
 * @param data 指向 student 結構體的 void 指標
 * 備註：當前 student 的 name 是陣列（非動態記憶體），故無需額外釋放；
 *      若 name 是 malloc 分配的 char*，需在此添加 free(stu->name)
 */
void free_student(void *data)
{
    student *stu = (student *)data;
    // 若後續修改為動態記憶體（如 stu->name = malloc(50)），需添加：
    // if (stu->name) {
    //     free(stu->name);
    //     stu->name = NULL;  // 避免野指標
    // }
    // 當前無動態記憶體，函數體可留空
}

