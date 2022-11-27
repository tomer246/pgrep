#include "types.h"

#ifndef PRINTS
#define PRINTS

// prints a matrix mXn of ints, given m,n 
void print_int_matrix(int **matrix, int height, int width);
// prints array of intlists
void print_int_list_array(intlist **matrix, int height);
// prints a single intlist
void print_int_list(intlist* list);

#endif // PRINTS
