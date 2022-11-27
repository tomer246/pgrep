#include "types.h"
#include "prints.h"
#include "general.h"
#include "automatas.h"

#ifndef REGEX
#define REGEX

#define NUM_CHARS           0x80             //without epsilon
#define NUM_CHARS_eps       (NUM_CHARS+2)    //with two epsilons
#define EPSILON1            NUM_CHARS        //(number 0x81 starting from 0)
#define EPSILON2            (NUM_CHARS+1)    //(number 0x82 starting from 0)

int num_states;                              //number of nfa states
intlist *new_states;                         //list of the new (to-dfa) states
intlist *accepting_states;                   //list of accepting states
int max_len_new_state;
int **nfa;                                   //nfa matrix
int **new_states_matrix;                     //new states matrix
intlist **states_reachable;                  //list of reachable states lists


int fill_nfa(char * regex, int nfa_state, int start_index, int end_index);
int activate_regex(char *regex);
int clean_regex();
int run_search(char *line);
int run_word(char *word);


#endif //REGEX
