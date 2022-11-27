#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prints.h"

// prints a matrix mXn of ints, given m,n 
void print_int_matrix(int **matrix, int height, int width){
    for (int i=0; i<height; i++){
        for (int j=0; j<width; j++){
            printf("%d, ", matrix[i][j]);
        }
        printf("\n");
    }
}

// prints array of intlist
void print_int_list_array(intlist **matrix, int height){
    int width;
    for (int i=0; i<height; i++){
        if (matrix[i] == NULL){
            continue;
        }
        width = matrix[i]->length;
        for (int j=0; j<width; j++){
            printf("%d, ", (matrix[i]->array)[j]);
        }
        puts("\n");
    }
}

// prints a single intlist
void print_int_list(intlist* list){
    for (int j=0; j<list->length; j++){
        printf("%d, ", (list->array)[j]);
    }
    puts("\n");
}

// prints an int array
void print_int_arr(int* arr, int length){
    for (int j=0; j<length; j++){
        printf("%d, ", arr[j]);
    }
    puts("\n");
}
