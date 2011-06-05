/*
 * Predictive_Parser.h
 *
 *  Created on: Jun 3, 2011
 *      Author: ahmedkotb
 */

#ifndef PREDICTIVE_PARSER_H_
#define PREDICTIVE_PARSER_H_

#include "CFG.h"
#include <map>
#include <set>
#include <queue>
#include "Parse_Table_Entry.h"
#include "Lexical_Analyser.h"
#include <stack>
#include <list>

#define INPUT_END_MARKER "#$#"

class Predictive_Parser {

private:

	CFG * grammar;

	//sets of first of each non terminal
	map<string,set<string>*> *first;

	//first set of each production of given nonterminal
	map<pair<string,int>,set<string>*> * subset_first;

	//sets of follow of each non terminal
	map<string,set<string>*> *follow;

	//parse table
	Parse_Table_Entry** parse_table;

	//terminals/non_terminals numbers
	map<string,int> terminals_num;
	map<string,int> non_terminals_num;


	void calc_first(string);
	void calc_follow();

	void generate_table();

public:
	Predictive_Parser(CFG *grammar);

	//parse a source file
	void parse(string rules_file_path , string source_file_path);

	void debug();
	virtual ~Predictive_Parser();
};

#endif /* PREDICTIVE_PARSER_H_ */
