#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "general.h"

// compare ints function
int comp (const void * elem1, const void * elem2) 
{
    int f = *((int*)elem1);
    int s = *((int*)elem2);
    if (f > s) return 1;
    if (f < s) return -1;
    return 0;
}

// sort an intlist's array
intlist* sort(intlist* x){
    qsort(x->array, x->length, sizeof(int), comp);
    return x;
}

// sort an array of ints
int* sort_array(int* x, int length){
    qsort(x, length, sizeof(int), comp);
    return x;
}

// check if an int val is in an intlist
int is_in_list(int val, intlist *list){
    if (list == NULL){
        return 0;
    }
    for (int i=0; i<list->length; i++){
        if (val == list->array[i]){
            return 1;
        }
    }
    return 0;
}

// check if an int val is in an array
int is_in_array(int val, int *array, int length){
    for (int i=0; i<length; i++){
        if (val == array[i]){
            return 1;
        }
    }
    return 0;
}

int is_in_char_array(int val, char *array, int length){
    for (int i=0; i<length; i++){
        if (val == (int)array[i]){
            return 1;
        }
    }
    return 0;
}

// check if two arrays are equal
int is_eq_array(int *array1, int *array2, int length1, int length2){
    if (length1!=length2)
        return 0;
    for (int i=0; i<length1; i++){
        if (array1[i]!=array2[i])
            return 0;
    }
    return 1;
}

// check if two intlists are equal
int is_eq_intlist(intlist *list1, intlist *list2){ //equal if arrays are equal.
    if (list1 == NULL){
        return (list2==NULL);
    }
    if (list2==NULL) //list1 isn't NULL
        return 0;
    return is_eq_array(list1->array, list2->array, list1->length, list2->length);
}

// recieves an array and returns an intlist with unique values, without 0's if possible
intlist *uniq(int *array, int length){
    sort_array(array, length);
    int num_uniq_elements = 1;
    for (int i=0; i<length; i++){ // calculate number of unique elements
        if (i==length-1){
            break;
        }
        if (array[i]<array[i+1]){
            num_uniq_elements++;
        }
    }

    intlist *ret = (intlist*)malloc(sizeof(intlist));
    
    // remove 0 from unique items if length>1 (except {0})
    int removed_zero=0;
    if(num_uniq_elements>1 && array[0]==0){
        removed_zero=1;
        num_uniq_elements-=1;
    }

    int *ret_arr = calloc(num_uniq_elements, sizeof(int));
    ret->length = num_uniq_elements;

    int curr_index = 0;
    if(!removed_zero){
        ret_arr[0] = array[0];
        curr_index = 1;
    }

    for (int i=0; i<length; i++){
        if (i==length-1){
            break;
        }
        if (array[i]<array[i+1]){
            ret_arr[curr_index] = array[i+1];
            curr_index++;
        }
    }
    ret->array = ret_arr;
    return ret;
}

int find_rparen(char* string, int lparen_index){
    int num_lparen = 1;
    int num_rparen = 0;
    int i = lparen_index+1;
    for (; i<strlen(string); i++){
        if (string[i] == '(')
            num_lparen ++;
        else if (string[i] == ')')
            num_rparen ++;
        if (num_lparen == num_rparen)
            return i;
    }
}

int correct_paren(char *string){
    int lparen = 0;
    int rparen = 0;
    int currbslash = 0;
    for (int i=0; i<strlen(string); i++)
    {
        if (string[i] == '(' && currbslash%2==0)
            lparen++;
        if (string[i] == ')' && currbslash%2==0)
            rparen++;
        if (rparen > lparen)
            return 0;
        if (string[i] == '\\')
            currbslash ++;
        else
            currbslash = 0;
    }
    return rparen == lparen;
}

intlist *intlist_union(intlist* int_list_index1, intlist* int_list_index2){
    intlist *int_list_index = (intlist*)malloc(sizeof(intlist));
    int max_length = int_list_index1->length + int_list_index2->length;
    int *arr = calloc(max_length, sizeof(int));
    for (int i=0; i<int_list_index1->length; i++){
        arr[i] = int_list_index1->array[i];
    }
    for (int i=0; i<int_list_index2->length; i++){
        arr[int_list_index1->length+i] = int_list_index2->array[i];
    }
    intlist *ptr = uniq(arr, max_length);
    int_list_index -> length = ptr->length;
    int_list_index -> array = ptr->array;
    free(ptr);
    return int_list_index;
}

void to_lower(char *line){
    for (int i=0; i<strlen(line); i++){
        line[i] = tolower(line[i]);
    }
}
