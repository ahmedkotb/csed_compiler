/*
 * NFA.cpp
 *
 *  Created on: Apr 22, 2011
 *      Author: ahmedkotb
 */

#include "NFA.h"

//private constructor
NFA::NFA(){
	alphabet = new set<INPUT_CHAR>();
}
//creates two states with one transition from the first to the second
NFA::NFA(INPUT_CHAR input_char){
	NFA();
	start_state = new NFA_State();
	final_state = new NFA_State();
	start_state->add_transition(input_char,final_state);

	//add the character to the NFA alphabet
	alphabet->insert(input_char);
	//set the accepting pattern of the final state
	final_state->set_accepting_pattern("" + input_char);
}

//creates a linked list like graph of nodes from the given sequence
NFA::NFA(string sequence){
	NFA();
	start_state = new NFA_State();
	NFA_State * ptr = start_state;
	for (unsigned int i = 0; i < sequence.length(); ++i) {
		NFA_State * new_state = new NFA_State();
		ptr->add_transition(sequence[i],new_state);
		//update the alphabet of the NFA
		alphabet->insert(sequence[i]);
		//update accepting pattern
		new_state->set_accepting_pattern(sequence.substr(0,i+1));
		//advance
		ptr = new_state;
	}
	final_state = ptr;
}

NFA::NFA(INPUT_CHAR start,INPUT_CHAR end){
	//start char must be <= end
	assert(start <= end);

	NFA();
	start_state = new NFA_State();
	final_state = new NFA_State();

	string acc_pattern = "";
	for (INPUT_CHAR i = start; i <= end; ++i) {
		start_state->add_transition(i,final_state);
		//generate accepting pattern
		if (i == end)
			acc_pattern += i;
		else
			acc_pattern += i + " | ";

		//update the alphabet
		alphabet->insert(i);
	}

	//set the accepting pattern of the final string to the OR of all characters
	final_state->set_accepting_pattern(acc_pattern);
}


void NFA::apply_plus_closure(){
	NFA_State* new_start_state = new NFA_State();
	NFA_State* new_final_state = new NFA_State();

	//attach the two new states
	new_start_state->add_transition(EPSILON,this->start_state);
	this->final_state->add_transition(EPSILON,new_final_state);

	//the repeating edge (that allows for more than one occurence of pattern)
	this->final_state->add_transition(EPSILON,new_start_state);

	//modifty starting and final state of this nfa
	this->start_state = new_start_state;
	this->final_state = new_final_state;

	//modify accepting pattern of final state
	this->final_state->set_accepting_pattern("( " + this->final_state->get_accepting_pattern() +" )+");

}

void NFA::apply_star_closure(){
	this->apply_plus_closure();
	//add the zero edge (that enables zero occurence of pattern)
	this->start_state->add_transition(EPSILON,this->final_state);
	//replace the + of the plus closure with *
	unsigned int index = this->final_state->get_accepting_pattern().rfind('+');
	// a + must be found as we have called apply_plus_closure()
	assert(index != string::npos);
	this->final_state->set_accepting_pattern(this->final_state->get_accepting_pattern().replace(index,1,"*"));
}

//joins the given NFA with this NFA
//TWO types of join exist NFA_JOIN_OR , NFA_JOIN_CONCATENATION
void NFA::join(NFA* nfa,int join_type){
	//check for null pointer
	assert(nfa != 0);
	//check for invalid join_type
	assert(join_type == NFA_JOIN_OR || join_type == NFA_JOIN_CONCATENATE);

	if (join_type == NFA_JOIN_OR)
		join_or(nfa);
	else
		join_concatenate(nfa);
}

void NFA::join_concatenate(NFA* nfa){
	//TODO implement this method
}

void NFA::join_or(NFA* nfa){
	//TODO revise this , and check for copy problem
	//STEPS
	//1 - copy nfa .. or not ?
	//2 - add 2 new states (start and end)
	//3 - add epsilon transitions from new start state to the start state of each nfa
	//	  add epsilon transitions from end states of each nfa to the new end state
	//4 - modify start and end state of this nfa
	//5 - union alphabet
	//6 - modify accepting pattern of total final state
	//----------------------------------------------------
	//1
	// this implementation doesn't copy the second nfa
	//2
	NFA_State *new_start_state = new NFA_State();
	NFA_State *new_final_state = new NFA_State();
	//3
	new_start_state->add_transition(EPSILON,this->start_state);
	new_start_state->add_transition(EPSILON,nfa->start_state);
	nfa->final_state->add_transition(EPSILON,new_final_state);
	this->final_state->add_transition(EPSILON,new_final_state);
	//4
	this->start_state = new_start_state;
	this->final_state = new_final_state;
	//5
	set<INPUT_CHAR>::iterator it;
	for (it = nfa->alphabet->begin(); it != nfa->alphabet->end(); it++)
		this->alphabet->insert(*it);
	//6
	this->final_state->set_accepting_pattern("( " + this->final_state->get_accepting_pattern() + " | " + nfa->final_state->get_accepting_pattern()+ " )");
}

NFA::~NFA() {
	// TODO Implement the destructor
	delete alphabet;
	//must make decision about the copy problem
	//delete start_state;
	//delete final_state;
}
