#include "doubly_linked_list.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    printf("\n雙向鏈表實現-模擬音樂播放器列表\n");

    // 1. 創建空鏈表
    printf(">> 步驟1: 創建空播放列表\n");
    doubly_linked_list *play_list = create_list();
    if (!play_list)
    {
        printf(">> 錯誤: 鏈表創建失敗, 程序退出\n");
        return EXIT_FAILURE; // 記憶體分配失敗, 退出程式
    }
    print_list_forward(play_list); // 此時鏈表爲空, 列印提示
    printf("\n");

    // 2. 尾部追加 4 首歌曲 (模擬添加初始歌單)
    printf(">> 步驟2: 尾部追加4首歌曲\n");
    append(play_list, (song){"As It Was", "Harry Styles", 167});  // 歌曲1: 167s
    append(play_list, (song){"Levitating", "Dua Lipa", 203});     // 歌曲2: 203s
    append(play_list, (song){"Good 4 U", "Olivia Rodrigo", 178}); // 歌曲3: 178s
    append(play_list,
           (song){"Blinding Lights", "The Weeknd", 200}); // 歌曲4: 200s
    print_list_forward(play_list); // 正向列印驗證追加結果
    printf("\n");

    // 3. 反向列印鏈表(驗證雙向鏈表的反向遍歷特徵)
    printf(">> 步驟3: 反向列印播放列表\n");
    print_list_backward(play_list); // 從最後一首歌到第一首歌
    printf("\n");

    // 4. 在指定歌曲後面插入新歌曲(在「Levitating」後插入「Industry Baby」)
    printf(">> 步驟4: 在「Levitating」後插入「Industry Baby」\n");
    node *levitating_node =
        find_by_title(play_list, "Levitating"); // 先查詢目標歌曲
    if (levitating_node)
    {
        // 插入成功: 調用 insert_after 完成插入
        insert_after(play_list, levitating_node,
                     (song){"Industry Baby", "Lil Nas X", 212});
    }
    else
    {
        printf(">> 錯誤: 未找到歌曲「Levitating」, 插入失敗!\n");
    }
    print_list_forward(play_list); // 列印驗證插入結果(此時共5首歌)
    printf("\n");

    // 5. 頭部插入歌曲(在最前面插入「First Class」)
    printf(">> 步驟5: 頭部插入歌曲「First Class」\n");
    prepend(play_list, (song){"First Class", "Jack Harlow", 174}); // 插入到頭部
    print_list_forward(
        play_list); // 列印驗證(此時共6首歌, 「First Class」在第一個)
    printf("\n");

    // 6. 刪除中間歌曲(刪除「Good 4 U」)
    printf(">> 步驟6: 刪除歌曲「Good 4 U」\n");
    node *good_4u_node = find_by_title(play_list, "Good 4 U"); // 查詢目標歌曲
    if (good_4u_node)
    {
        delete_node(play_list, good_4u_node); // 刪除成功
    }
    else
    {
        printf(">> 錯誤: 未找到歌曲「Good 4 U」, 刪除失敗\n");
    }
    print_list_forward(play_list); // 列印驗證(此時共5首歌)
    printf("\n");

    // 7. 刪除頭結點(刪除最前面的「First Class」)
    printf(">> 步驟7: 刪除頭結點「First Class」\n");
    node *head_node = play_list->head; // 直接獲取頭結點(無需查詢)
    if (head_node)
    {
        delete_node(play_list, head_node); // 刪除頭結點
    }
    else
    {
        printf(">> 錯誤: 鏈表爲空, 無法刪除頭節點\n");
    }
    print_list_forward(
        play_list); // 列印驗證(此時共4首歌, 新頭節點爲「As It Was」)
    printf("\n");

    // 8. 刪除尾節點(刪除最後面的「Blinding Lights」)
    printf(">> 步驟8: 刪除尾節點「Blinding Lights」\n");
    node *tail_node = play_list->tail; // 直接獲取尾節點(無需查詢)
    if (tail_node)
    {
        delete_node(play_list, tail_node); // 刪除尾節點
    }
    else
    {
        printf(">> 錯誤: 鏈表爲空, 無法刪除尾節點\n");
    }
    print_list_forward(
        play_list); // 列印驗證(此時共3首歌曲, 新尾節點爲「Industry Baby」)
    printf("\n");

    // 9. 查詢不存在的歌曲(驗證查詢失敗場景)
    printf(">> 步驟9: 查詢不存在的歌曲「Bad Guy」\n");
    node *noe_existent_node = find_by_title(play_list, "Bad Guy");
    if (noe_existent_node)
    {
        printf(">> 資訊: 找到歌曲「Bad Guy」\n");
    }
    else
    {
        printf(">> 資訊: 未找到歌曲「Bad Guy」(符合預期)\n");
    }

    // 10. 釋放所有記憶體(程式結束前清理資源, 避免泄露)
    printf(">> 步驟10: 釋放播放列表中所有記憶體\n");
    free_list(play_list);
    play_list = NULL;              // 將指針置空, 避免野指針
    print_list_forward(play_list); // 驗證釋放結果(此時鏈表爲空)

    printf("\n====== 所有演示步驟執行完畢 ======\n");

    return EXIT_SUCCESS;
}

