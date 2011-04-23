//============================================================================
// Name        : csed_compiler.cpp
// Author      : Ahmed Kotb
// Version     :
// Copyright   : All Rights Reserved KotbCorp
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "NFA.h"
using namespace std;

int main() {
	cout << "The Best Compiler isA" << endl;

	//testing NFA

	NFA nfa1('x','z');
	NFA nfa2('b');

	cout << "===================================================" << endl;

	nfa1.join(&nfa2,NFA_JOIN_OR);

	NFA nfa3('c');
	nfa1.join(&nfa3,NFA_JOIN_CONCATENATE);

	nfa1.finalize_NFA(0);
	//nfa accept the pattern (x | y | z | b)c
	nfa1.debug();

	return 0;
}
