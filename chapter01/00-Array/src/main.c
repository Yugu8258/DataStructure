#include "DynamicArray.h"

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    printf("\n動態數組的初步實現.\n");

    int capacity = 5;

    DynamicArray *my_array = create_array(capacity);

    array_append(my_array, 10);
    array_append(my_array, 20);
    array_append(my_array, 30);
    array_append(my_array, 40);
    array_append(my_array, 50);

    print_array(my_array);

    destory_array(my_array);

    return EXIT_SUCCESS;
}

