/*
 * NFA_State.h
 *
 *  Created on: Apr 22, 2011
 *      Author: ahmedkotb
 */

#ifndef NFA_STATE_H_
#define NFA_STATE_H_

#define INPUT_CHAR signed short
#define EPSILON -1

#include <string>
#include <vector>
using namespace std;

class NFA_State {

private:
	int id;
	string accepting_pattern;
	bool is_accepting;
	int token_id;

public:
	NFA_State();

	//add transition with specified input
	void add_transition(INPUT_CHAR,NFA_State *);

	//get transition with specified input
	vector<NFA_State *> get_transitions(INPUT_CHAR);

	void set_accepting_pattern(string);
	string get_accepting_pattern();
	virtual ~NFA_State();
};

#endif /* NFA_STATE_H_ */
