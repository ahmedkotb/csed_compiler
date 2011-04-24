/*
 * NFA.h
 *
 *  Created on: Apr 22, 2011
 *      Author: ahmedkotb
 */

#ifndef NFA_H_
#define NFA_H_

#define NFA_JOIN_OR 0
#define NFA_JOIN_CONCATENATE 1


#include "NFA_State.h"
#include <set>
#include <queue>
#include <assert.h>
#include <sstream>

class NFA {
private:

	NFA_State * start_state;
	NFA_State * final_state;

	//holds number of states in this NFA
	int states_count;

	//combined NFA are a special case of nfa as it has many accepting states
	//and can't be joined with other nfa(s)
	bool combined_NFA;

	//to be used in NFA-DFA conversion
	set<INPUT_CHAR>* alphabet;

	//private method used to concatenate this NFA with another NFA
	void join_concatenate(NFA*);
	//private method used to OR this NFA with another NFA
	void join_or(NFA*);

	//private method used to traverse the given graph and preappend the given string
	//to the beginning accepting pattern of each state
	void concatenate_pattern(NFA*,string);

	//private method used to number the states of the NFA
	//it gives each state a unique id starting from 0
	void number_states();

	//Empty NFA Constructor is private only
	NFA();

public:

	//One Character NFA Constructor
	NFA(INPUT_CHAR);
	//Sequence NFA Constructor
	NFA(string);
	//Range NFA Constructor
	NFA(INPUT_CHAR start,INPUT_CHAR end);

	void join(NFA*,int);

	void apply_star_closure();
	void apply_plus_closure();

	//getters
	set<INPUT_CHAR>* get_alphabet();
	int get_states_count();
	bool is_combined_NFA();

	//set the final state of the NFA as accepting state and assign the give token ID to it
	void finalize_NFA(int);

	//for debugging purpose
	//prints the entire NFA
	void debug();

	//static method used to create combined NFA of group of NFA(s)
	static NFA* create_combined_NFA(vector<NFA*> *);
	virtual ~NFA();
};

#endif /* NFA_H_ */
