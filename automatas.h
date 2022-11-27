#include "prints.h"
#include "general.h"
#include "types.h"

#ifndef AUTOMATAS
#define AUTOMATAS

int calc_num_states(char * regex, int  start_index, int end_index);
int create_nfa(char * regex);
void free_nfa();
intlist *get_reachable(int state, int get_recur_result, int num_iteration);
void calc_states_reachable();
int create_new_states_from_reachables();
void free_new_states();
void free_new_stats_matrix();
int get_state(intlist *new_state, char ch, int spl);
int fill_new_states();
void set_accepting_states();

#endif //AUTOMATAS
