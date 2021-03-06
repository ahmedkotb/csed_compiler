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
#include <sstream>
#include "Parsing_output.h"
//Constants
#define  RANGE '-'
#define  IN_EPSLON "\L"
#define RE 0
#define RD 1
#define START_PUNCTUATION "["		//The first tokens of the punctuation line
#define START_KEYWORD "{"			//The first token in the keywords line
#define DELIMETER "\\"
//Operators
#define OR "|"
#define CONCATINATE " "
#define CLOSURE "*"
#define POSITIVE_CLOSURE "+"
#define SPECIAL_NUM 12
#define CONCATINATION_SYMBOL " "
#define OPERATOR_NUM 4
#define WS_NUM 3
#define WHITE_SPACES_NAME "White Space"
#define WHITE_SPACE_ID 0
using namespace std;

class Parser {
public:
    Parser();
    virtual ~Parser();
    Parsing_output* parse(string file_name);
private:
	stack <string> op_Stack;
	vector <string>*output;
	vector <string>* input;
    set<string >* keyWordMap;
    vector <string>* punctuation;	//The punctuation in the system
    vector<string>* RE_names;
    vector<vector<string>* >* RE_list;
    map<string, vector<string>*>* RDMap;
    vector<string>* lan_tokens;
    vector<NFA*>* NFAS;
    //Methods
    void install_Keywords();
    void install_punctuation();
    void select_line_type(vector <string>* tokens);
    void addRegularDefinition();
    void gotOper(string opThis, int prec1);
	void gotParen(string ch);
	NFA* get_post_fix(string name, vector<string>* value,bool isRD);
	bool isOperator(string token);
	NFA *createByBaseCase(string str);
	NFA *stringToNFA(vector <string> *in);
	bool isSpecialChar(char c);
	void split(string line);
	NFA* addTokens();
	vector<NFA*>* getNFAS();
	vector<string>* getTokens();
	void add_white_spaces();
};

#endif	/* TREE_BUILDER_H */
