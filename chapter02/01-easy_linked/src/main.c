#include <stdio.h>
#include <stdlib.h>

// 定義節點結構
// 1. data int
// 2. 指向下一個節點的指針
typedef struct Node
{
    int data;
    // 結構體嵌套
    struct Node *next;
} Node;

Node *create_node(int data);

void list_append(Node **head_ref, int data);

void prepend_node(Node **head_ref, int data);

void print_list(Node *head);

Node *find_node(Node *head, int data);

void update_node(Node *head, int old_data, int new_data);

void delete_node(Node **head_ref, int data);

void list_free(Node **head_ref);

int main(void)
{
    puts("單向鏈表的簡易實現");

    Node *head = NULL;

    list_append(&head, 10);
    list_append(&head, 20);
    list_append(&head, 30);

    print_list(head);

    list_append(&head, 40);

    print_list(head);

    prepend_node(&head, 5);

    print_list(head);

    list_free(&head);

    head = NULL;

    print_list(head);

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

