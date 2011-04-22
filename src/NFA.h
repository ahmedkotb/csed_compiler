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
#include <assert.h>

class NFA {
private:

	NFA_State * start_state;
	NFA_State * final_state;

	//to be used in NFA-DFA conversion
	set<INPUT_CHAR>* alphabet;

	NFA();

	void join_concatenate(NFA*);
	void join_or(NFA*);
public:
	//Empty NFA Constructor is not allowed
	//NFA();

	//One Character NFA Constructor
	NFA(INPUT_CHAR);
	//Sequence NFA Constructor
	NFA(string);
	//Range NFA Constructor
	NFA(INPUT_CHAR start,INPUT_CHAR end);

	void join(NFA*,int);

	void apply_star_closure();
	void apply_plus_closure();

	virtual ~NFA();
};

#endif /* NFA_H_ */
