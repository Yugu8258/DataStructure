#include <stdio.h>
#include <stdlib.h>

/**
 * 节点结构定义
 * @param data 存储的整数数据
 * @param next 指向下一个节点的指针
 */
typedef struct Node
{
    int data;
    // 結構體嵌套
    struct Node *next;
} Node;

/**
 * 创建新节点
 * @param data 节点存储的数据
 * @return 成功返回新节点指针, 失败返回NULL
 */
Node *create_node(int data);

/**
 * 在链表尾部追加节点
 * @param head_ref 指向头指针的指针 (用于修改头节点)
 * @param data 要添加的数据
 */
void list_append(Node **head_ref, int data);

/**
 * 在链表头部插入节点
 * @param head_ref 指向头结点的指针
 * @param data 要添加的数据
 */
void prepend_node(Node **head_ref, int data);

/**
 * 打印链表所有数据
 * @param head 链表头节点
 */
void print_list(Node *head);

/**
 * 查找指定数据的节点
 * @param head 链表头节点
 * @param data 要查找的数据
 * @return 找到返回节点指针, 否则返回NULL
 */
Node *find_node(Node *head, int data);

/**
 * 更新指定数据的节点
 * @param head 链表头结点
 * @param old_data 旧数据 (待更新)
 * @param new_data
 */
void update_node(Node *head, int old_data, int new_data);

/**
 * 刪除指定數據的節點
 * @param head_ref 指向頭指針的指針
 * @param data 要刪除的數據
 */
void delete_node(Node **head_ref, int data);

/**
 * 釋放鏈表所有節點內存
 * @param head_ref 指向頭指針的指針 (釋放後置爲NULL)
 */
void list_free(Node **head_ref);

int main(void)
{
    puts("單向鏈表的簡易實現");

    Node *head = NULL;

    // 測試1: 尾部追加節點
    puts("\n\t[測試1: 尾部追加節點]");
    list_append(&head, 10);
    list_append(&head, 20);
    list_append(&head, 30);
    print_list(head); // 預期: 10 -> 20 -> 30 -> NULL

    // 測試2: 繼續追加節點
    puts("\n\t[測試2: 繼續追加節點]");
    list_append(&head, 40);
    print_list(head); // 預期: 10 -> 20 -> 30 -> 40 -> NULL

    // 測試3: 頭部插入節點
    puts("\n\t[測試3: 頭部插入節點]");
    prepend_node(&head, 5);
    print_list(head); // 預期: 5 -> 10 -> 20 -> 30 -> 40 -> NULL

    // 測試4: 查找節點
    puts("\n\t[測試4: 查找節點]");
    Node *found = find_node(head, 20);
    if (found != NULL)
    {
        printf("找到節點: %d\n", found->data); // 預期: 找到節點 20
    }
    else
    {
        printf("未找到節點\n");
    }

    found = find_node(head, 100);
    if (found != NULL)
    {
        printf("找到節點: %d\n", found->data);
    }
    else
    {
        printf("未找到節點\n"); // 預期: 找到節點
    }

    // 測試5: 更新節點
    puts("\n\t[測試5: 更新節點]");
    update_node(head, 30, 300); // 存在的節點
    update_node(head, 99, 999); // 不存在的節點
    print_list(head);           // 預期: 5 -> 10 -> 20 -> 300 -> 40 -> NULL

    // 測試6: 刪除節點(中間節點)
    puts("\n\t[測試6: 刪除中間節點]");
    delete_node(&head, 20);
    print_list(head); // 預期: 5 -> 10 -> 300 -> 40 -> NULL

    // 測試7: 刪除頭結點
    puts("\n\t[測試7: 刪除頭節點]");
    delete_node(&head, 5);
    print_list(head); // 預期: 10 -> 300 -> 40 -> NULL

    // 測試8: 刪除尾節點
    puts("\n\t[測試8: 刪除尾節點]");
    delete_node(&head, 40);
    print_list(head); // 預期: 10 -> 300 -> NULL

    // 測試9: 刪除不存在的節點
    puts("\n\t[測試9: 刪除不存在的節點]");
    delete_node(&head, 999);
    print_list(head); // 預期: 10 -> 300 -> NULL

    // 測試10: 釋放鏈表
    puts("\n\t[測試10: 釋放鏈表]");
    list_free(&head);
    print_list(head); // 預期: 當前鏈表: NULL

    return EXIT_SUCCESS;
}

Node *create_node(int data)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node)
    {
        fprintf(stderr, "錯誤: 內存分配失敗!\n");
        return NULL;
    }

    new_node->data = data;
    new_node->next = NULL;

    return new_node;
}

// head_ref 是指向head指針的指針
// 這樣我們才能夠在鏈表爲空的時候, 修改head
void list_append(Node **head_ref, int data)
{
    Node *new_node = create_node(data);
    if (new_node == NULL)
    {
        return;
    }

    if (*head_ref == NULL)
    {
        *head_ref = new_node;
        return;
    }

    Node *last = *head_ref;
    while (last->next != NULL)
    {
        last = last->next;
    }

    last->next = new_node;
}

void prepend_node(Node **head_ref, int data)
{
    Node *new_node = create_node(data);

    new_node->next = *head_ref;
    *head_ref = new_node;
}

void print_list(Node *head)
{
    Node *current = head;

    printf("當前鏈表:");
    while (current != NULL)
    {
        printf("%d -> ", current->data);
        current = current->next;
    }

    printf("NULL\n");
}

Node *find_node(Node *head, int data)
{
    Node *current = head;
    // 遍歷查找數據
    while (current != NULL)
    {
        if (current->data == data)
        {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

void update_node(Node *head, int old_data, int new_data)
{
    Node *node_to_update = find_node(head, old_data);
    if (node_to_update != NULL)
    {
        node_to_update->data = new_data;
        printf("數據 %d 已經成功更新至 %d.", old_data, new_data);
    }
    else
    {
        printf("未找到!\n");
    }
}

void delete_node(Node **head_ref, int data)
{
    Node *temp = *head_ref;
    Node *prev = NULL;

    // 如果刪除的是頭節點
    if (temp != NULL && temp->data == data)
    {
        *head_ref = temp->next;
        free(temp);
        return;
    }

    // 如果刪除的節點在尾部或者中間
    while (temp != NULL && temp->data != data)
    {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL)
    {
        return;
    }

    prev->next = temp->next;

    free(temp);
}

void list_free(Node **head_ref)
{
    if (head_ref == NULL)
    {
        return;
    }

    Node *current = *head_ref;

    Node *next_node;

    while (current != NULL)
    {
        next_node = current->next;
        free(current);
        current = next_node;
    }

    *head_ref = NULL;
}

