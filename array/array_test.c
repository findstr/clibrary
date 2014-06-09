/**
=========================================================================
 Author: findstr
 Email: findstr@sina.com
 File Name: array_test.c
 Description: (C)  2014-04  findstr
   
 Edit History: 
   2014-04-27    File created.
=========================================================================
**/
#include <stdio.h>
#include "array.h"

int main()
{
        int i;
        int tmp;
        int *a;

        struct array *arr = array_create(sizeof(int));

        for (i = 0; i < 100; i++)
                array_push(arr, &i);

        for (i = 0; i < array_size(arr); i++) {
                array_get(arr, i, &tmp);
                printf("%d--", tmp);
        }

        printf("\n----------------\n");
        array_erase(arr, 30);

        for (i = 0; i < array_size(arr); i++) {
                array_get(arr, i, &tmp);
                printf("%d--", tmp);
        }

        a = (int *)array_ptr(arr, 3);

        printf("\n--aa--%d--\n", *a);

        return 0;
}
