#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "automatas.h"
#include "regex.h"

// calculate number of states (of nfa, before epsilon moves)
int calc_num_states(char * regex, int start_index, int end_index)
{
    int total = 0;
    int rparen_index;
    int curr_ch;
    int next_ch;
    for (int i=start_index; i<=end_index; i++){
        curr_ch = regex[i];
        next_ch = (i<end_index) ? regex[i+1] : 'a';
        if (curr_ch == '\\'){
            total++;
            i++;
            continue;
        }
        if (i!=strlen(regex) && (next_ch=='?' | next_ch=='*')){
            total++;
            i++;
            continue;
        }
        if (i!=strlen(regex) && (next_ch=='+')){
            total+=2;
            i++;
            continue;
        }
        if (curr_ch == '('){
            rparen_index = find_rparen(regex, i);
            next_ch = regex[rparen_index+1];
            switch (next_ch){
                case '?':
                    total += calc_num_states(regex, i+1, rparen_index-1) + 1;
                    i = rparen_index+1;
                    continue;
                case '*':
                    total += calc_num_states(regex, i+1, rparen_index-1) + 2;
                    i = rparen_index+1;
                    continue;
                case '+':
                    total += calc_num_states(regex, i+1, rparen_index-1) + 1;
                    i = rparen_index+1;
                    continue;
            }
        }

        total += 1; // else, a regular letter
        continue; 
    }
    return total;
}

// creating the nfa matrix
int create_nfa(char * regex) 
{
    num_states = calc_num_states(regex, 0 , strlen(regex)-1)+2;
    nfa = (int**)malloc(num_states*sizeof(int*));
    for (int i=0; i<num_states; i++){
        nfa[i] = calloc(NUM_CHARS_eps, sizeof(int));
    }
}

// free the nfa
void free_nfa(){
    for (int i=0; i<num_states; i++){
        free(nfa[i]);
    }
    free(nfa);
}

// get all the reachable states from a state
intlist *get_reachable(int state, int get_recur_result, int num_iteration){
    // if entry already exists for the state, then either it is a legit one, i.e index==0
    // or it is a temporary one for the recursion, i.e index==iteration.
    // otherwise we don't want it.
    if (states_reachable[state]!=NULL){ 
        int index = states_reachable[state]->index; 
        if (index == 0 || (get_recur_result == 1 && index == num_iteration))
            return states_reachable[state];
    }

    int *reachables;
    intlist *reachable_list = (intlist*)malloc(sizeof(intlist));

    // Otherwise, make sure states_reachable[state] != NULL now so we don't get stuck in a loop trying to get epsilons steps
    reachables = &state;
    reachable_list->array = reachables;
    reachable_list->length = 1;
    reachable_list->index = num_iteration;
    states_reachable[state] = reachable_list;

    int epsilon_state = nfa[state][EPSILON1];
    // if there aren't any epsilon steps, return [state]
    if (epsilon_state == 0){            
        reachables = calloc(1, sizeof(int));
        *reachables = state;
        reachable_list->array = reachables;
        reachable_list->length = 1;
        return reachable_list;
    }

    intlist *epsilon_reachables;
    int epsilon_state2 = nfa[state][EPSILON2];
    if (epsilon_state2 != 0){ // if there are two epsilon steps, get the union of their reachables
        epsilon_reachables = intlist_union(get_reachable(epsilon_state, 1, num_iteration), get_reachable(epsilon_state2, 1, num_iteration));
    }
    else{
        epsilon_reachables = get_reachable(epsilon_state, 1, num_iteration); // otherwise, get the reachables of the one epsilon step
    }

    if (is_in_list(state, epsilon_reachables)){ // If state is in (reachables from epsilon)=A, return A
        reachables = calloc(epsilon_reachables->length, sizeof(int));
        memcpy(reachables, epsilon_reachables->array, (epsilon_reachables->length)*sizeof(int));
        reachable_list->array = reachables;
        reachable_list->length = epsilon_reachables->length;
        return sort(reachable_list);
    }

    reachables = calloc(epsilon_reachables->length+1, sizeof(int));
    memcpy(reachables, epsilon_reachables->array, (epsilon_reachables->length)*sizeof(int));
    reachables[epsilon_reachables->length] = state;
    reachable_list->array = reachables;
    reachable_list->length = epsilon_reachables->length+1;
    return sort(reachable_list);
} //TODO there's a loop of epsilons, dont create many new states that are the same. for example, (x?v?)*

// create the sturcture states_reachable, of the reachable states from each state
void calc_states_reachable(){
    states_reachable = calloc(num_states, sizeof(intlist*));
    for (int state=num_states-1; state>=0; state--){
        states_reachable[state] = get_reachable(state, 0, state+1);
        states_reachable[state]->index = 0;
    }
}

// free states_reachable
void free_states_reachable(){
    for (int i=0; i<num_states; i++){
        free(states_reachable[i]->array);
        free(states_reachable[i]);
    }
    free(states_reachable);
}

int create_new_states_from_reachables()
{
    new_states = (intlist*)malloc(sizeof(intlist));
    new_states->length = 0;
    for (int i=0; i<num_states; i++){
        if (states_reachable[i]->length != 1)
            new_states->length+=1;
    }
    new_states->array = (int*)malloc(new_states->length * sizeof(int));
    int curr_pos=0;
    for (int i=0; i<num_states; i++){
        states_reachable[i]->index = 0;

        if (states_reachable[i]->length != 1){
            new_states->array[curr_pos++] = i;
            states_reachable[i]->index = curr_pos; // starting from 1 as 0 indicates no new state
        }
        if (max_len_new_state<states_reachable[i]->length)
            max_len_new_state = states_reachable[i]->length;
    }
    max_len_new_state++;

    new_states_matrix = (int**)malloc(new_states->length*sizeof(int*));
    for (int i=0; i<new_states->length; i++){
        new_states_matrix[i] = calloc(NUM_CHARS, sizeof(int));
    }
}

void free_new_states(){
    free(new_states->array);
    free(new_states);
}

void free_new_stats_matrix(){
    for (int i=0; i<new_states->length; i++){
        free(new_states_matrix[i]);
    }
    free(new_states_matrix);
}

// gets a new state and a character and returns the f(state, character)
// new_state [x,y,z] => E[x] U E[y] U E[z]
// f([x,y,z], char) => f(E[x], char) U f(E[y], char) U f(E[z], char)
int get_state(intlist *new_state, char ch, int ch_ind){

    int *state_union = calloc(max_len_new_state, sizeof(int));
    int union_index=0; // index for the union
    int *reachables;
    int nfa_state;
    for (int i=0; i<new_state->length; i++){
        nfa_state = nfa[new_state->array[i]][ch];           // new_state [x,y,z] =>
        reachables = states_reachable[nfa_state]->array;    // E[x] / E[y] / E[z]
        for (int j=0; j<states_reachable[nfa_state]->length; j++){
            if (!is_in_array(reachables[j], state_union, max_len_new_state)){
                state_union[union_index++] = reachables[j];
            }
        }
    }
    intlist *uniq_union = uniq(state_union, max_len_new_state); // throws zeros unless all zeros
    free(state_union);

    /*
    if (uniq_union->length != 1 || (uniq_union->length == 1 && uniq_union->array[0]!=0)){
        puts("this is the list: ");
        print_int_list(uniq_union);
        print_int_list(new_state);
        printf("===================================== %d %c\n", ch_ind, ch);
        print_int_list(uniq_union);
        puts("\n\n");
    }
    */

    for (int k=0; k<num_states; k++){
        if (is_eq_array(uniq_union->array, states_reachable[k]->array, uniq_union->length, states_reachable[k]->length)){
            free(uniq_union->array);
            free(uniq_union);
            return k;
        }
    }
    exit(0);
}

int fill_new_states()
{
    int state;
    for (int i=0; i<new_states->length; i++){
        for(int chr=0; chr<NUM_CHARS; chr++){
            state = new_states->array[i];
            new_states_matrix[i][chr] = get_state(states_reachable[state], chr, i);
        }
    }
}

void set_accepting_states(){
    int how_many = 0;
    for (int i=0; i<num_states; i++){
        if (is_in_list(num_states-1, states_reachable[i]))
            how_many++;
    }
    int *accepting_states_arr = calloc(how_many, sizeof(int));
    int curr_pos = 0;
    for (int i=0; i<num_states; i++){
        if (is_in_list(num_states-1, states_reachable[i]))
            accepting_states_arr[curr_pos++] = i;
    }
    accepting_states = (intlist*)malloc(sizeof(intlist));
    accepting_states->array = accepting_states_arr;
    accepting_states->length = how_many;
}

void free_accepting_states(){
    free(accepting_states->array);
    free(accepting_states);
}