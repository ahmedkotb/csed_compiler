/*
 * CFG.h
 *
 *  Created on: Jun 3, 2011
 *      Author: ahmedkotb
 */

#ifndef CFG_H_
#define CFG_H_

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include "Token.h"
#include "Lexical_Analyser.h"

#define HASH "HASH"
#define NON_TERMINAL "NON_TERMINAL"
#define TERMINAL "TERMINAL"
#define OR_CHAR "OR"
#define EQUAL "EQUAL"
#define EPSILON_CHAR "EPSILON"
#define EPSILON_TOKEN "#EPSILON#"
#define GRAMMAR_FILE_PATH "./grammer_syntax"

using namespace std;

class CFG {
private:
	//start Symbol of grammar S
	string start_symbol;
	//terminals of grammar
	set<string>* terminals;
	//non terminal of grammar
	set<string>* nonterminals;
	//productions
	map<string,vector<vector<string>*>*>* productions;

	//parse grammar file
	void parse_file(string);
public:
	CFG(string grammar_file_path);

	//getters
	string get_start_symbol();
	set<string>* get_terminals();
	set<string>* get_nonterminals();
	map<string,vector<vector<string>*>*>* get_productions();

	//prints the grammar to console for debugging
	void debug();
	virtual ~CFG();
};

#endif /* CFG_H_ */
