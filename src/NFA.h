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

	//to be used in NFA-DFA conversion
	set<INPUT_CHAR>* alphabet;

	//private method used to concatenate this NFA with another NFA
	void join_concatenate(NFA*);
	//private method used to OR this NFA with another NFA
	void join_or(NFA*);

	//private method used to traverse the given graph and preappend the given string
	//to the beginning accepting pattern of each state
	void concatenate_pattern(NFA*,string);

public:
	//Empty NFA Constructor is not allowed

	//One Character NFA Constructor
	NFA(INPUT_CHAR);
	//Sequence NFA Constructor
	NFA(string);
	//Range NFA Constructor
	NFA(INPUT_CHAR start,INPUT_CHAR end);

	void join(NFA*,int);

	void apply_star_closure();
	void apply_plus_closure();

	set<INPUT_CHAR>* get_alphabet();

	virtual ~NFA();
};

#endif /* NFA_H_ */
