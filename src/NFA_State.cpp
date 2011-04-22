/*
 * NFA_State.cpp
 *
 *  Created on: Apr 22, 2011
 *      Author: ahmed
 */

#include "NFA_State.h"

NFA_State::NFA_State() {
	//the id will be used as an identifier in the conversion to DFA state
	id = 0;
	//EPSILON is considered the accepting pattern for a new state ( \L == EPSILON )
	accepting_pattern = "\L";

	//state is not considered (Token Acceptor) at creation
	is_accepting = false;

	//Accepting Token ID
	token_id = -1;

	//create map transition
	transitions = new multimap<INPUT_CHAR, NFA_State*> ();
}

void NFA_State::add_transition(INPUT_CHAR input, NFA_State* state) {
	transitions->insert(pair<INPUT_CHAR, NFA_State*> (input, state));
}

vector<NFA_State *>* NFA_State::get_transitions(INPUT_CHAR input) {

	//check epsilon input
	if (input == EPSILON)
		return epislon_closure(this);
	else
		return get_transitions_helper(input); //normal case
}

vector<NFA_State *>* NFA_State::get_transitions_helper(INPUT_CHAR input) {

	vector<NFA_State *> *result;
	result = new vector<NFA_State *> ();
	return_it = transitions->equal_range(input);
	//iterate over the input range
	for (iterate = return_it.first; iterate != return_it.second; ++iterate) {
		NFA_State *s = (*iterate).second;
		result->push_back(s);
	}
	return result;
}

vector<NFA_State *>* NFA_State::epislon_closure(NFA_State* s) {

	//get first level neighbors
	vector<NFA_State*> *parent = s->get_transitions_helper(EPSILON);

	vector<NFA_State*>::iterator i, j;
	//base case is when s is accepting state
	if (s->is_accepting)
		return parent;

	vector<NFA_State*>::iterator iterate;
	for (iterate = parent->begin(); iterate != parent->end(); ++iterate) {
		NFA_State *s2 = (*iterate);

		//check self loop
		if (s == s2)
			return parent;

		vector<NFA_State*> *child = epislon_closure(s2);

		//merge parent and child
		parent->insert(parent->end(), child->begin(), child->end());

		//check for duplication
		removeDuplicates(parent);

	}
	return parent;
}


void NFA_State::set_accepting_pattern(string accepting_pattern) {
	this->accepting_pattern = accepting_pattern;
	is_accepting = true;
}

void NFA_State::set_id(int new_id) {
	id = new_id;
}

string NFA_State::get_accepting_pattern() {
	return this->accepting_pattern;
}

//print state informations
string NFA_State::show() {
	string info;
	cout << "ID = " << (id >= 0 ? id : (int) this) << "\n";
	/*cout << "Accepting Pattern = " << accepting_pattern << "\n";
	 cout << "State Type = " << (is_accepting ? "Accepting" : "Normal") << "\n";
	 cout << "Regular Expression ID ="
	 << (token_id >= 0 ? token_id : (int) this) << "\n";
	 cout << "Transitions :" << "\n";
	 cout << "-------------" << "\n";
	 for (iterate = transitions->begin(); iterate != transitions->end(); ++iterate) {
	 cout << (char) (iterate->first >= 0 ? (iterate->first) : 'E')
	 << "  =>  " << iterate->second->id << endl;
	 }*/
	return info;
}

NFA_State::~NFA_State() {
	for (iterate = transitions->begin(); iterate != transitions->end(); ++iterate) {
		delete iterate->second;
		iterate->second = 0;
	}
	transitions->clear();
}


int main2() {
	NFA_State *s;
	NFA_State *v;
	NFA_State *w;
	NFA_State *q;

	v = new NFA_State();
	s = new NFA_State();
	w = new NFA_State();
	q = new NFA_State();

	s->set_id(1);
	v->set_id(2);
	w->set_id(3);
	q->set_id(4);

	s->add_transition(EPSILON, v);
	s->add_transition(EPSILON, q);
	s->add_transition(EPSILON, v);
	s->add_transition(EPSILON, q);
	s->add_transition(EPSILON, q);
	v->add_transition(EPSILON, w);
	s->add_transition(EPSILON, s);
	//s->show() ;
	v->add_transition('b', s);/*
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
		vector<NFA_State *> *get_transitions(INPUT_CHAR);

		void set_accepting_pattern(string);
		string get_accepting_pattern();
		virtual ~NFA_State();
	};

	#endif /* NFA_STATE_H_ */

	//v->set_accepting_pattern("ssss");
	//v->show();

	vector<NFA_State*> *vec = s->get_transitions(EPSILON);
	//cout << "returned" << endl;
	vector<NFA_State*>::iterator iterate2;
	for (iterate2 = vec->begin(); iterate2 != vec->end(); ++iterate2) {
		NFA_State *s = (*iterate2);
		s->show();
	}

	return 0;
}


