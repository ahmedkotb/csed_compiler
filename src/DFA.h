/* 
 * File:   DFA.h
 * Author: Ahmed El-Sharkasy
 *
 * Created on 23 أبريل, 2011, 09:49 م
 */

#ifndef DFA_H
#define	DFA_H

#include "NFA_State.h"

class DFA {
public:
    DFA();
    DFA(const DFA& orig);
    virtual ~DFA();

    // mov method which takes a state and input character and returns a vector of the transitions
    vector<NFA_State *> * mov(NFA_State* current_state, char input_character);

    // create a DFA state representing the given NFA states and sets the accepting pattern of the DFA state
    void create_DFA_state(NFA_State* states);
private:

    NFA_State * start_state;

    //holds all input alphabet
    vector <char> alphabet;

    //hashmap returns a DFA state representing the given ids of NFA states
    multimap <vector<int*>, NFA_State*> states_IDS;

    //vector for all DFA states
    vector<NFA_State*>* all_states;

};

#endif	/* DFA_H */

