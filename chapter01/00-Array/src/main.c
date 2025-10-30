#include "DynamicArray.h"

#include <stdio.h>
#include <stdlib.h>

// 一個輔助函數, 讓輸出更漂亮
void print_head(const char *title);

int main(int argc, char *argv[])
{
    printf("\n動態數組的簡單實現\n");

    // === 1. 初始化 ===
    print_head("1. 初始化數組");

    int capacity = 5; // 初始化容量爲 5

    DynamicArray *my_array = create_array(capacity);

    array_append(my_array, 10);
    array_append(my_array, 20);
    array_append(my_array, 30);
    array_append(my_array, 40);
    array_append(my_array, 50);

    printf("初始化數組創建完畢");
    print_array(my_array);
    printf("注意: 當前Size和capacity相等. 下一次添加將觸發'擴容'.\n");

    array_append(my_array, 60);
    printf("添加 60 之後, 觸發了自動擴容!\n");
    print_array(my_array);

    // === 2. 讀取 (Read) - O(1) ===
    print_head("2. 讀取操作 (O(1))");
    size_t read_index = 2;
    Data *value_ptr = array_read(my_array, read_index);
    if (value_ptr)
    {
        printf("讀取索引 %zu 的值...\n", read_index);
        printf("成功! 值爲: %d\n", *value_ptr);
        puts("解說: 這非常塊, 因爲C語言直接通過 `base_address + index * "
             "sizeof(int)` 計算出內存地址, 一步到位!\n");
    }
    else
    {
        puts("讀取失敗!\n");
    }

    // === 3. 更新 (Update) - O(1) ===
    print_head("3. 更新操作 (O(0))");
    size_t update_index = 3;
    Data new_value = 99;
    printf("將索引 %zu 的值從 %d 更新爲 %d...\n", update_index,
           *array_read(my_array, update_index), new_value);
    array_update(my_array, update_index, new_value);
    print_array(my_array);
    puts("解說: 更新同樣是O(1)操作, 因爲它也是通過地址計算直接定位並覆蓋值.\n");

    // === 4. 插入 (Insert) - O(n) ===
    print_head("4. 插入操作 (O(n)) - 數組的痛點");
    size_t insert_index = 1;
    Data insert_value = 777;
    printf("在索引 %zu 處插入新值 %d...", insert_index, insert_value);
    printf("操作前:\n");
    print_array(my_array);

    array_insert(my_array, insert_index, insert_value);

    printf("操作後:\n");
    print_array(my_array);
    printf("解說: 爲了插入 %d, 從索引 %zu 開始的所有元素 (20, 30, 99, 50, 60) "
           "都必須向后移動一位置, 這是一個O(n)操作, 當數組非常大的時候, "
           "會非常耗時.",
           insert_value, insert_index);

    // === 5. 刪除 (Delete) - O(n) ===
    print_head("5. 刪除操作 (O(n)) - 數組的另一個痛點");
    size_t delete_index = 4;
    printf("刪除索引 %zu 的元素 (%d)...\n", delete_index,
           *array_read(my_array, delete_index));
    printf("操作前:\n");
    print_array(my_array);

    array_delete(my_array, delete_index);

    printf("操作後:\n");
    print_array(my_array);
    printf("解說: 爲了刪除 %d 元素, "
           "它後面的所有元素都必須向前移動來填補空缺, 這同樣是O(n)操作.\n",
           *array_read(my_array, delete_index));

    // === 6. 銷燬數組 (非常重要)===
    destory_array(my_array);
    my_array = NULL; // 置空, 防止指針懸掛
    puts("\n顯示結束, 已釋放所有內存\n");

    return EXIT_SUCCESS;
}

void print_head(const char *title)
{
    printf("\n=================================================\n");
    printf("\t%s\n", title);
    printf("=================================================\n");
}

