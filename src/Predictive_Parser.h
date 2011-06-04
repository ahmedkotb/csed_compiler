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

#define INPUT_END_MARKER "#$#"

class Predictive_Parser {
private:
	CFG * grammar;
	map<string,set<string>*> *first;
	map<string,set<string>*> *follow;
	void calc_first(string);
	void calc_follow();
public:
	Predictive_Parser(CFG *grammar);

	void debug();
	virtual ~Predictive_Parser();
};

#endif /* PREDICTIVE_PARSER_H_ */
