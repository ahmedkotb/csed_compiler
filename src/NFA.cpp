/*
 * NFA.cpp
 *
 *  Created on: Apr 22, 2011
 *      Author: ahmedkotb
 */

#include "NFA.h"

//creates two states with one transition from the first to the second
NFA::NFA(INPUT_CHAR input_char){
	alphabet = new set<INPUT_CHAR>();
	start_state = new NFA_State();
	final_state = new NFA_State();
	start_state->add_transition(input_char,final_state);

	//add the character to the NFA alphabet
	alphabet->insert(input_char);
	//set the accepting pattern of the final state
	stringstream ss;
	ss << input_char;
	final_state->set_accepting_pattern(ss.str());
}

//creates a linked list like graph of nodes from the given sequence
NFA::NFA(string sequence){
	alphabet = new set<INPUT_CHAR>();
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

	alphabet = new set<INPUT_CHAR>();
	start_state = new NFA_State();
	final_state = new NFA_State();

	stringstream acc_pattern_stream;
	for (INPUT_CHAR i = start; i <= end; ++i) {
		start_state->add_transition(i,final_state);
		//generate accepting pattern
		if (i == end)
			acc_pattern_stream << i;
		else
			acc_pattern_stream << i << " | ";
		//update the alphabet
		alphabet->insert(i);
	}

	//set the accepting pattern of the final string to the OR of all characters
	final_state->set_accepting_pattern(acc_pattern_stream.str());
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

//make a bfs traversal of the 2nd graph and pre-appends the given pattern to each of the states accepting patterns
void NFA::concatenate_pattern(NFA * nfa,string pattern){
	//TODO implement this method
//	set<NFA_State *> visited;
//	queue<NFA_State *> queue;
//	visited.insert(nfa->start_state);
//	queue.push(nfa->start_state);
//	NFA_State * state;
//	while (!queue.empty()){
//		state = queue.front();
//		queue.pop();
//		state->set_accepting_pattern(pattern + state->get_accepting_pattern());
//	}
}

void NFA::join_concatenate(NFA* nfa){
	//TODO revise this , and check for copy problem
	//STEPS
	//1 - copy nfa .. or not ?
	//2 - join final state in this nfa with the start state in the given nfa with epsilon transition
	//3 - union alphabet
	//4 - Accepting pattern :
	//  --- there is a problem here . all the acc_pattern of the states of the 2nd graph will need to be changed
	//  --- Suggested solution , traverse the 2nd graph and edit the accepting pattern
	//  --- but since accepting_pattern is not needed for the program to work correctly (used only for debugging)
	//  --- we can enable it just during the coding phase and disable it when we finish so that join_concatenate
	//  --- remains o(1) instead of o(n) where n is the number of states in the 2nd nfa
	//----------------------------------------------------

	//1
	// this implementation doesn't copy the second nfa
	//2
	this->final_state->add_transition(EPSILON,nfa->start_state);
	//3
	set<INPUT_CHAR>::iterator it;
	for (it = nfa->alphabet->begin(); it != nfa->alphabet->end(); it++)
		this->alphabet->insert(*it);
	//4 fix the accepting pattern
	//disable the following code in the project final phase
	concatenate_pattern(nfa,this->final_state->get_accepting_pattern());
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

set<INPUT_CHAR>* NFA::get_alphabet(){
	set<INPUT_CHAR> * alphabet_copy = new set<INPUT_CHAR>();
	set<INPUT_CHAR>::iterator it;
	for (it = alphabet->begin(); it != alphabet->end(); it++)
		alphabet_copy->insert(*it);
	return alphabet_copy;
}
NFA::~NFA() {
	// TODO Implement the destructor
	delete alphabet;
	//must make decision about the copy problem
	//delete start_state;
	//delete final_state;
}
