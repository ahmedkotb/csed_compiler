/*
 * NFA_State.cpp
 *
 *  Created on: Apr 22, 2011
 *      Author: ahmedkotb
 */

#include "NFA_State.h"

NFA_State::NFA_State() {
	// TODO complete the constructor
	//the id will be used as an indentifier in the conversion to DFA state
	id = 0;
	//EPSILON is considered the accepting pattern for a new state ( \L == EPSILON )
	accepting_pattern = "\L";

	//state is not considered (Token Acceptor) at creation
	is_accepting = false;

	//Accepting Token ID
	token_id = -1;
}


void NFA_State::add_transition(INPUT_CHAR input,NFA_State * state){
	//TODO implement this method
}

vector<NFA_State *> NFA_State::get_transitions(INPUT_CHAR input){
	//TODO implement this method
	vector<NFA_State *> result;
	return result;
}


void NFA_State::set_accepting_pattern(string accepting_pattern){
	this->accepting_pattern = accepting_pattern;
}

string NFA_State::get_accepting_pattern(){
	return this->accepting_pattern;
}
NFA_State::~NFA_State() {
	//TODO implement the destructor
}
