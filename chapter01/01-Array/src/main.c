#include "DynamicArray.h"

#include <stdio.h>

// 定义我们自己的回调函数
// 1. 这是一个知道如何打印Student的函数
// 它的函数签名必须和函数指针的定义完全匹配
void print_student(const void *data);

int main(void)
{
    puts("\n動態數組的學生類別實現\n");

    printf("--- 测试 Student 动态数组 ---\n");
    DynamicArray *student_list = create_array(2);

    Student s1 = {101, "Alice"};
    Student s2 = {102, "Bob"};
    array_append(student_list, s1);
    array_append(student_list, s2);

    // 调用通用的 print_array，并把“如何打印学生”的函数传给它!
    printf("打印学生名单：\n");
    print_array(student_list, &print_student); // & 是可选的，但更清晰
    destroy_array(student_list);
    student_list = NULL;

    return 0;
}

void print_student(const void *data)
{
    // 接收到的data是一个void*指针，我们需要将它转换为我们真正的类型
    const Student *s_ptr = (const Student *)data;
    printf("Student: {id: %d, name: \"%s\"}", s_ptr->id, s_ptr->name);
}

