/* 
 * File:   DFA.h
 * Author: Ahmed El-Sharkasy
 *
 * Created on 23 أبريل, 2011, 09:49 م
 */

#ifndef DFA_H
#define	DFA_H

#include "NFA_State.h"
#include "NFA.h"

class DFA {
public:
    DFA();
    DFA(NFA* nfa);
    DFA(const DFA& orig);
    virtual ~DFA();

    // create a DFA state representing the given NFA states and sets the accepting pattern of the DFA state
    NFA_State* create_DFA_state(vector<NFA_State*>* states);

    // return DFA start state
    NFA_State* get_start_state();

    // return the IDs of the given NFA states
    vector<int> get_IDS(vector<NFA_State*>* states);

    // takes a victor of integer and return an integer representing the result of hashing
    int hash(vector<int> ids);

    //method used to number the states of the DFA
    //it gives each state a unique id starting from 0
    void number_states();

    //describes the entire DFA
    void debug();

private:
    // mov method which takes a state and input character and returns a vector of the transitions
    vector<NFA_State *> * mov(NFA_State* current_state, INPUT_CHAR input_character);

    // takes NFA and converts it to DFA
    void convert_NFA_to_DFA(NFA* nfa);

    // helper method used in NFA to DFA conversion
    void get_DFA(vector<NFA_State*>* states,NFA* nfa);

    //the start state of the DFA
    NFA_State * start_state;

    //map returns a DFA state representing the given id of NFA states
    map <int, NFA_State*> states_IDS;

};

#endif	/* DFA_H */
