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
using namespace std;

int main() {
    cout << "The Best Compiler isA" << endl;

    //testing NFA

    NFA nfa1('x', 'z');
    NFA nfa2('b');

    cout << "===================================================" << endl;

    nfa1.join(&nfa2, NFA_JOIN_OR);

    NFA nfa3('c');
   nfa1.join(&nfa3, NFA_JOIN_CONCATENATE);

//    nfa1.finalize_NFA(0);
    //nfa accept the pattern (x | y | z | b)c
   // nfa1.debug();

    cout << "===================================================" << endl;
   // NFA nfaA('a');
    //nfaA.finalize_NFA(0);
//    NFA nfaB('b');
//    nfaB.finalize_NFA(1);
//    vector<NFA*> vector;
//    vector.push_back(&nfaA);
//    vector.push_back(&nfaB);
    //NFA * combined = NFA::create_combined_NFA(&vector);
   // combined->debug();
    
    cout<<"Test:#+++======================================="<<endl;
    nfa1.number_states();
    DFA* dfa = new DFA(&nfa1);
    dfa->number_states();
    cout << dfa->get_start_state()->get_description() << endl;
    cout<<"Debug:"<< dfa->get_start_state()->get_transitions('x')->at(0)->get_description()<<endl;
    
    return 0;
}
