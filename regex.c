#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "regex.h"
#include "prints.c"
#include "general.c"
#include "automatas.c"

#define MAX_REG_LEN         100
#define NUM_WHITESPACES     6
#define NUM_DIGITS          10
char *regex;

char whitespaces[NUM_WHITESPACES] = {' ', '\n', '\r', '\v', '\f', '\t'};
#define new_line            0xa

int fill_nfa(char * regex, int nfa_state, int start_index, int end_index)
{
    printf("%d\n", start_index);
    char curr_ch;
    char next_ch;
    int rparen_index;
    int next_state;
    for (int i=start_index; i<=end_index; i++){
        curr_ch = regex[i];
        printf("%c\n", curr_ch);

        next_ch = i<end_index ? regex[i+1] : 'a';

        if (curr_ch == '\\'){
            switch (next_ch){
                case 's':
                    for (int j=0; j<NUM_WHITESPACES; j++)
                        nfa[nfa_state][whitespaces[j]] = nfa_state+1;
                    nfa_state ++;
                    i++;
                    break;
                case 'S':
                    for (int j=0; j<NUM_CHARS; j++){
                        if (!is_in_char_array(j, whitespaces, NUM_WHITESPACES))
                            nfa[nfa_state][j] = nfa_state+1;
                    }
                    nfa_state ++;
                    i++;
                    break;
                case 'd':
                    for (int j='0'; j<='9'; j++)
                        nfa[nfa_state][j] = nfa_state+1;
                    nfa_state ++;
                    i++;
                    break;
                case 'D':
                    for (int j=0; j<NUM_CHARS; j++){
                        if (j<'0' || j>'9')
                            nfa[nfa_state][j] = nfa_state+1;
                    }
                    nfa_state ++;
                    i++;
                    break;
                case 'w':
                    for (int j='0'; j<='9'; j++)
                        nfa[nfa_state][j] = nfa_state+1;
                    for (int j='a'; j<='z'; j++)
                        nfa[nfa_state][j] = nfa_state+1;
                    for (int j='A'; j<='Z'; j++)
                        nfa[nfa_state][j] = nfa_state+1;
                    nfa_state ++;
                    i++;
                    break;
                case 'W':
                    for (int j=0; j<NUM_CHARS; j++){
                        if (!((j>='0' && j<='9') || (j>='a' && j<='z') || (j >='A' && j<='Z')))
                            nfa[nfa_state][j] = nfa_state+1;
                    }
                    nfa_state ++;
                    i++;
                    break;
                default:
                    nfa[nfa_state][next_ch] = nfa_state+1;
                    nfa_state ++;
                    i++;
                    break;
            }
            continue;
        }
        
        switch (next_ch){
            case '?': 
                nfa[nfa_state][curr_ch] = nfa_state+1;
                nfa[nfa_state][EPSILON1] = nfa_state+1;
                nfa_state = nfa_state+1;
                i++;
                continue;
            case '*':
                nfa[nfa_state][curr_ch] = nfa_state;
                nfa[nfa_state][EPSILON1] = nfa_state+1;
                nfa_state = nfa_state+1;
                i++;
                continue;
            case '+':
                nfa[nfa_state][curr_ch] = nfa_state+1;
                nfa[nfa_state+1][curr_ch] = nfa_state+1;
                nfa[nfa_state+1][EPSILON1] = nfa_state+2;
                nfa_state = nfa_state+2;
                i++;
                continue;
            default:
                break;
        }

        switch (curr_ch){
            case '.':
                for (int j=0; j<NUM_CHARS; j++){
                    if (j != new_line)
                        nfa[nfa_state][j] = nfa_state+1;
                }
                nfa_state = nfa_state + 1;
                break;
            case '(':
                rparen_index = find_rparen(regex, i);
                next_ch = rparen_index<end_index ? regex[rparen_index+1] : 'a';
                switch(next_ch){
                    case '*':
                        next_state = fill_nfa(regex, nfa_state+1, i+1, rparen_index-1);
                        nfa[nfa_state][EPSILON1] = nfa_state+1;
                        nfa[nfa_state][EPSILON2] = next_state+1;
                        nfa[next_state][EPSILON1] = nfa_state;
                        nfa_state = next_state+1;
                        i = rparen_index + 2;
                        break;
                    case '?':
                        next_state = fill_nfa(regex, nfa_state+1, i+1, rparen_index-1);
                        nfa[nfa_state][EPSILON1] = nfa_state+1;
                        nfa[nfa_state][EPSILON2] = next_state;
                        nfa_state = next_state;
                        i = rparen_index + 2;
                        break;
                    case '+':
                        next_state = fill_nfa(regex, nfa_state, i+1, rparen_index-1);
                        nfa[next_state][EPSILON1] = nfa_state;
                        nfa[next_state][EPSILON2] = next_state+1;
                        nfa_state = next_state+1;
                        i = rparen_index + 2;
                        break;
                    default: // TODO
                        break;
                }
                break;

            case '^':
            case '[':
            case ']':
            case '|':
            case '-': //TODO
            default:
                nfa[nfa_state][curr_ch] = nfa_state+1;
                nfa_state ++;
                break;
        }   
    }
    return nfa_state;
}

int activate_regex(char *regex){
    printf("=============== CREATING NFA =============\n");

    create_nfa(regex);
    fill_nfa(regex, 1, 0, strlen(regex)-1);
    print_int_matrix(nfa, num_states, NUM_CHARS_eps);

    printf("=============== CREATING DFA =============\n");
    printf("=============== CALCULATING REACHABLES =============\n");

    calc_states_reachable(); // if same elements then same object. problems with free. need to fix
    print_int_list_array(states_reachable, num_states);

    printf("=============== CREATING NEW STATES ============\n");

    create_new_states_from_reachables();
    puts("\n");
    print_int_list(new_states);

    printf("=============== FILLING NEW STATES ============\n");

    fill_new_states();
    print_int_matrix(new_states_matrix, new_states->length, NUM_CHARS);

    printf("=============== SETTING ACCEPTING STATES ============\n");

    set_accepting_states();
    puts("\n");
    print_int_list(accepting_states);

    printf("=============== RUNNING ON SOME INPUT =============\n");

}

int clean_regex(){
    printf("================= CLEANUP ===============\n");
    free_accepting_states();
    free_new_stats_matrix();
    free_new_states();
    free_states_reachable();
    free_nfa();
}

int run_search(char *line)
{
    int length = strlen(line);
    int next_state = 1;
    int is_new_state=0;

    for (int i=0; i<length; i++){
        is_new_state = (states_reachable[next_state]->index != 0);
        if (is_new_state)
            next_state = (states_reachable[next_state]->index)-1;

        if (is_new_state){
            next_state = new_states_matrix[next_state][line[i]];
        }
        else{
            next_state = nfa[next_state][line[i]];
        }
        
        if (next_state == 0)
            return 0;
        if (is_in_list(next_state, accepting_states))
            return 1;
    }
}

int run_word(char *word)
{
    int length = strlen(word);
    int next_state = 1;
    int is_new_state=0;
    printf("step 0 starts at state: 1\n");

    for (int i=0; i<length; i++){
        printf("%c\n", word[i]);
        is_new_state = (states_reachable[next_state]->index != 0);
        if (is_new_state)
            next_state = (states_reachable[next_state]->index)-1;

        if (is_new_state){
            puts("new\n");
            next_state = new_states_matrix[next_state][word[i]];
        }
        else{
            puts("old\n");
            next_state = nfa[next_state][word[i]];
        }

        printf("step %d leads to state: %d\n", i+1, next_state);
        /*
        if (next_state == 0)
            return 0;
        if (is_in_list(next_state, accepting_states))
            return 1;
        */
    }

    return is_in_list(next_state, accepting_states);
}

int main(int argc, char* argv[]){
    FILE *fptr = fopen(argv[1], "r");
    char buf[MAX_REG_LEN] = {0};
    fread(buf, 1, 100, fptr);
    fclose(fptr);
    if(buf[strlen(buf)-1] == '\n'){
        buf[strlen(buf)-1] = '\x00';
    }
    regex = buf;
    activate_regex(buf);

    char buf2[100] = {0};
    fgets(buf2, MAX_REG_LEN, stdin);
    if(buf2[strlen(buf2)-1] == '\n'){
        buf2[strlen(buf2)-1] = '\x00';
    }
    int accepted = run_word(buf2);
    printf("%s\n", (accepted? "Accepted" : "Rejected"));

    clean_regex();
}
