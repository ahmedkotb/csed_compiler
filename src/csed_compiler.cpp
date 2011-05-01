//============================================================================
// Name        : csed_compiler.cpp
// Author      : Ahmed Kotb
// Version     :
// Copyright   : All Rights Reserved KotbCorp
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "NFA.h"
#include "DFA.h"
#include "Parser.h"
using namespace std;

void do_tests();

int main() {
	cout << "The Best Compiler isA" << endl;
	do_tests();
	return 0;
}
void nfa_dfa_tests(){
	cout << "===================================================" << endl;
	//Testing Simple NFA operations (ORing and concatenation)
	NFA nfa1('x', 'z');
	NFA nfa2('b');

	nfa1.join(&nfa2, NFA_JOIN_OR);
	NFA nfa3('c');
	nfa1.join(&nfa3, NFA_JOIN_CONCATENATE);
	nfa1.finalize_NFA(0);
	//nfa accept the pattern (x | y | z | b)c
	//nfa1.debug();
	assert(nfa1.get_states_count() == 8);
	//Result == SUCCESS
	cout << "===================================================" << endl;
	//Testing simple combined NFA creation
	NFA nfaA('a');
	nfaA.finalize_NFA(0);
	NFA nfaB('b');
	nfaB.finalize_NFA(1);
	vector<NFA *> vec;
	vec.push_back(&nfaA);
	vec.push_back(&nfaB);
	NFA * combined = NFA::create_combined_NFA(&vec);
	//combined->debug();
	assert(combined->get_states_count() == 5);
	//Result == SUCCESS
	cout << "===================================================" << endl;
	//Testing NFA to DFA conversion
	NFA nfa4('x', 'z');
	DFA* dfa = new DFA(&nfa4);
	//dfa->debug();
	assert(dfa->get_states_count() == 2);
	//Result == SUCCESS
	cout << "===================================================" << endl;
	//Testing NFA to DFA conversion AND MINIMIZATION
	// Example in compilers book page 155
	// (a|b)*abb
	// the dfa is found at page 156
	NFA nfa5('a');
	nfa5.join(new NFA('b'),NFA_JOIN_OR);
	nfa5.apply_star_closure();
	nfa5.join(new NFA("abb"),NFA_JOIN_CONCATENATE);
	nfa5.finalize_NFA(0);
	//nfa5.debug();
	DFA * result = new DFA(&nfa5);
	assert(result->get_states_count() == 5);
	//Result == SUCCESS  [5 states as in the book page 156]
	result->minimize();
	assert(result->get_states_count() == 4);
	//result->debug();
	//Result == SUCCESS  [4 states as in the book page 184]
	cout << "===================================================" << endl;
	//Testing Cycles in NFA to DFA conversion
	NFA nfa6('c');
	nfa6.join(new NFA('d'),NFA_JOIN_OR);
	//to add a to the alphabet
	nfa6.join(new NFA('a'),NFA_JOIN_CONCATENATE);
	nfa6.number_states();
	//adding a cycle
	set<NFA_State *> * other_states = nfa6.get_start_state()->get_transitions(EPSILON);
	set<NFA_State *>::iterator it = other_states->begin();
	NFA_State * state1=*(it);
	//cout << "state 1 id = " << state1->get_id() << endl;
	++it;
	NFA_State * state2=*(it);
	//cout << "state 2 id = " << state2->get_id() << endl;
	state1->add_transition('a',nfa6.get_start_state());
	state2->add_transition('a',state1);
	DFA * cycle_dfa = new DFA(&nfa6);
	//cycle_dfa->debug();
	assert(cycle_dfa->get_states_count() == 4);
	//Result == FAIL @ rev 40
	//[Infinite loop due to duplicates in DFA conversion epsilon closure (each cell var contained duplicates)]
	//Result == SUCESS @ rev 41
	cout << "===================================================" << endl;
	//testing combined NFA to DFA conversion
	//testing mutli accept state nfa
	NFA nfaAA('a');
	nfaAA.finalize_NFA(0);
	NFA nfaBB('b');
	nfaBB.finalize_NFA(1);
	vector<NFA *> vector_of_nfa;
	vector_of_nfa.push_back(&nfaAA);
	vector_of_nfa.push_back(&nfaBB);
	NFA * combined_NFA = NFA::create_combined_NFA(&vector_of_nfa);
	assert(combined_NFA->get_states_count() == 5);
	DFA * test_Combined_DFA = new DFA(combined_NFA);
	//test_Combined_DFA->debug();
	assert(test_Combined_DFA->get_states_count() == 3);
}
void parsing_tests(){
	//Testing the parser
	Parser* p = new Parser();
	//ON WINDOWS
	//Parsing_output* combined_nfa = p->parse("tests\t1");
	//ON Linux
	Parsing_output* output = p->parse("./tests/t1");
	vector<string>::iterator it;
	vector<string>* tokens = output->get_lan_tokens();
	cout << "Tokens :" << endl;
	for(it = tokens->begin(); it != tokens->end() ; it++)
		cout << (*it)<<endl;
	NFA* nfa= output->get_combinedNFA();
	DFA* dfa = new DFA(nfa);
	dfa->minimize();
	dfa->debug();
}
void do_tests(){
	nfa_dfa_tests();
	parsing_tests();
}
