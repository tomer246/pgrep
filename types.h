#ifndef TYPES
#define TYPES

typedef struct intlist {
    int *array;
    int length;
    int index; // Has different uses during the regex setup. At the "get reachables" phase it is used to mark the iteration number
               // to avoid infinite loops. Then it is used to mark each reachables list's index in the "new states" list, as 0 value
               // suggests that there is no new state matching the state.
} intlist;

#endif // TYPES
