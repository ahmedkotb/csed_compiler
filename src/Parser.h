/* 
 * File:   Tree_Builder.h
 * Author: 3mara
 *
 * Created on April 21, 2011, 11:26 PM
 */

#ifndef PARSER_H
#define	PARSER_H


#include <iostream>
#include <fstream>
#include <stack>
#include <vector>
#include <string>
#include "NFA.h"
#include <map>
//Constants
#define RE 0
#define RD 1
#define START_PUNCTUATION "["		//The first tokens of the punctuation line
#define START_KEYWORD "{"			//The first token in the keywords line
//Operators
#define OR "|"
#define CONCATINATE " "
#define CLOSURE "*"
#define POSITIVE_CLOSURE "+"
#define SPECIAL_NUM 11
#define CONCATINATION_SYMBOL " "
#define OPERATOR_NUM 4
using namespace std;

class Parser {
public:
    stack <string> op_Stack;
    vector <string>*output;
    vector <string>* input;
    Parser();
    virtual ~Parser();
    void parse(string line);
    void addTokens();
	vector<NFA*>* getNFAS();
	vector<string>* getTokens();
private:
    set<string >* keyWordMap;
    vector <string>* punctuation;	//The punctuation in the system
    vector<string>* RE_names;
    vector<vector<string>* >* RE_list;
    map<string, vector<string>*>* RDMap;
    vector<string>* lan_tokens;
    vector<NFA*>* NFAS;
    //Methods
    void installKeywords();
    void select_line_type(vector <string>* tokens);
    void addRegularDefinition();
    void gotOper(string opThis, int prec1);
	void gotParen(string ch);
	NFA* get_post_fix(string name, vector<string>* value,bool isRD);
	bool isOperator(string token);
	NFA *createByBaseCase(string str);
	NFA *stringToNFA(vector <string> *in);
	bool isSpecialChar(char c);
};

#endif	/* TREE_BUILDER_H */
