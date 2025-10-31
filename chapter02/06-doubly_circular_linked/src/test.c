#include "circular_doubly_linked_list.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 播放列表歌曲結構
 */
typedef struct play_list_song
{
    char *title;    // 歌曲標題
    char *artist;   // 藝術家
    int duration_s; // 時長(秒)
} play_list_song;

/**
 * @brief 跨平臺字符串複製
 * @param s 原字符串
 * @return 複製後的新字符串 (需手動釋放)
 */
static char *_protable_strdup(const char *s);

/**
 * @brief 歌曲數據釋放函數 (適配free_func)
 * @param data 歌曲指針
 */
static void _free_song(void *data);

/**
 * @brief 按標題比較歌曲 (適配compare_func)
 * @param data1 歌曲1
 * @param data2 歌曲2
 * @return 字符串比較結果 (strcmp返回值)
 */
static int _compare_song_by_title(const void *data1, const void *data2);

/**
 * @brief 打印歌曲信息 (適配action_func)
 * @param data 歌曲指針
 * @param context 計數器指針 (用於序號顯示)
 */
static void _print_song_action(void *data, void *context);

/**
 * @brief 創建歌曲 (輔助函數)
 * @param title 標題
 * @param artist 藝術家
 * @param duration 時長(秒)
 * @return 歌曲指針, 失敗返回NULL
 */
static play_list_song *_create_song(const char *title, const char *artist,
                                    int duration);

int main(int argc, char *argv[])
{

    puts("==== 雙向循環鏈表的ADT實現 ====");

    // 1. 創建播放列表 (內存池容量10)
    printf(">> [步驟1] 創建播放列表...\n");

    int capacity = 10;

    circular_doubly_linked_list *play_list = list_create(capacity, _free_song);
    if (!play_list)
    {
        fprintf(stderr, "播放列表創建失敗, 程序退出\n");
        return EXIT_FAILURE;
    }

    // 2. 添加歌曲 (尾部追加3首, 頭部插入1首)
    printf(">> [步驟2] 添加歌曲到播放列表...");

    list_append(play_list, _create_song("Bohemian Rhapsody", "Queen", 355));

    list_append(play_list,
                _create_song("Stariway to Hwaven", "Led Zeppelin", 482));

    list_append(play_list, _create_song("Hotel California", "Eagles", 391));

    list_prepend(play_list, _create_song("Imagine", "John Lennon", 183));

    // 3. 打印初始播放列表
    printf("\n>> [步驟3] 顯示播放列表 (共%d首)\n",
           (int)list_get_size(play_list));

    int counter = 1;

    circular_doubly_list_node *current_node = list_get_head_node(play_list);

    play_list_song *current_song =
        (play_list_song *)circular_doubly_list_node_get_data(current_node);

    printf("當前播放: [%s]\n", current_song->title);

    list_rotate_forward(play_list); // 向前旋轉 (下一首)

    current_node = list_get_head_node(play_list);

    current_song =
        (play_list_song *)circular_doubly_list_node_get_data(current_node);

    printf("下一首: [%s]\n", current_song->title);

    list_rotate_forward(play_list); // 在旋轉一次

    current_node = list_get_head_node(play_list);

    current_song =
        (play_list_song *)circular_doubly_list_node_get_data(current_node);

    printf("再下一首: [%s]\n", current_song->title);

    // 5. 打印旋轉後的列表
    printf("\n>> [步驟5] 刪除歌曲 [Stariway to Hwaven] ...\n");

    counter = 1;

    play_list_song search_key = {"Stariway to Hwaven", NULL,
                                 0}; // 僅用標題作爲關鍵字

    circular_doubly_list_node *node_to_delete =
        list_find_node(play_list, &search_key, _compare_song_by_title);

    if (node_to_delete)
    {
        list_delete_node(play_list, node_to_delete);
        printf("刪除後列表的大小: %zu\n", list_get_size(play_list));
    }
    else
    {
        printf("未找到歌曲 [Stariway to Hwaven]\n");
    }

    // 打印刪除後的列表
    printf("\n刪除後的播放列表:\n");

    counter = 1;

    list_for_each(play_list, _print_song_action, &counter);

    // 7. 銷燬播放列表
    printf("\n>> [步驟7] 銷燬播放列表...\n");

    list_destroy(&play_list);

    printf("播放列表銷燬後的狀態: %s\n",
           (play_list == NULL) ? "已置空" : "未置空");

    printf("\n==== 測試結束 ====\n");

    return EXIT_SUCCESS;
}

static char *_protable_strdup(const char *s)
{
    if (!s)
    {
        return NULL;
    }

    size_t len = strlen(s) + 1;

    char *new_str = (char *)malloc(len);
    if (new_str)
    {
        memcpy(new_str, s, len);
    }

    return new_str;
}

static void _free_song(void *data)
{
    play_list_song *song = (play_list_song *)data;
    if (song)
    {
        printf("[free_song] 釋放歌曲: [%s] - %s\n", song->title, song->artist);
        free(song->title);
        free(song->artist);
        free(song);
    }
}

static int _compare_song_by_title(const void *data1, const void *data2)
{
    const play_list_song *song1 = (const play_list_song *)data1;
    const play_list_song *song2 = (const play_list_song *)data2;

    return strcmp(song1->title, song2->title);
}

static void _print_song_action(void *data, void *context)
{
    play_list_song *song = (play_list_song *)data;
    int *counter = (int *)context;

    if (song && counter)
    {
        printf("\t%d. [%s] - %s (%d秒)\n", (*counter)++, song->title,
               song->artist, song->duration_s);
    }
}

static play_list_song *_create_song(const char *title, const char *artist,
                                    int duration)
{
    play_list_song *song = (play_list_song *)malloc(sizeof(*song));
    if (!song)
    {
        fprintf(stderr, "[_create_song] 錯誤: 歌曲內存分配失敗\n");
        return NULL;
    }

    song->title = _protable_strdup(title);
    song->artist = _protable_strdup(artist);
    song->duration_s = duration;

    // 內存分配失敗時回滾
    if (!song->title || !song->artist)
    {
        free(song->title);
        free(song->artist);
        free(song);
        return NULL;
    }

    return song;
}

