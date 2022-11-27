#include "types.h"

#ifndef GENERAL
#define GENERAL

// compare ints function
int comp (const void * elem1, const void * elem2);
// sort an intlist's array
intlist* sort(intlist* x);
// sort an array of ints
int* sort_array(int* x, int length);
// check if an int val is in an intlist
int is_in_list(int val, intlist *list);
// check if an int val is in an array
int is_in_array(int val, int *array, int length);
// check if an int val is in a char array
int is_in_char_array(int val, char *array, int length);
// check if two arrays are equal
int is_eq_array(int *array1, int *array2, int length1, int length2);
// check if two intlists are equal
int is_eq_intlist(intlist *list1, intlist *list2);
// recieves an array and returns an intlist with unique values, without 0's if possible
intlist *uniq(int *array, int length);
// finds right parenthesis given left one's index
int find_rparen(char* string, int lparen_index);
// returns the union of two int_list_indexes
intlist *intlist_union(intlist* int_list_index1, intlist* int_list_index2);

#endif // GENERAL
