//
//  main.c
//  week3Assigment1
//
//  Created by Anh Luong on 7/30/16.
//  Copyright © 2016 Anh Luong. All rights reserved.
//

#include <stdio.h>

int partition(int *array, int length, int pivot);

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    return 0;
}

int choosePivot(int *A, int n)
{
    return 0;
}

int partition(int *array, int length, int pivot)
{
    return 0;
}

int rSelect(int *array, int length, int order)  // n is lenght of A, i is the order statistic
{
    int p, j;   // p is the index of pivot, j is the new index of p (index of the pivot in the partitioned array)
    
    if (length == 1)
        return *array;
    
    p = choosePivot(array, length);
    j = partition(array, length, p);
    
    if (j == order)
        return *(array + j);
    else if (j > order)
        return rSelect(array, j - 1, order);
    else    // if (j < i)
        return rSelect(array, length - j, order - j);
}