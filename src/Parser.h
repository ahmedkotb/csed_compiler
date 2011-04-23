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
//Constants
#define RE 0
#define RD 1
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
    Parser(vector <string>* in);
    vector <string>* input;
    Parser();
    virtual ~Parser();
    void gotOper(string opThis, int prec1);
    void gotParen(string ch);
    void scan();
    bool isOperator(string token);
    NFA *createByBaseCase(string str);
    NFA *stringToNFA(vector <string> *in);
    void split(string line);
    bool isSpecialChar(char c);
private:
    set<string >* keyWordMap;
    map<string, vector<string> >* RDMap;
    stack <NFA*> operand_stack;
};

#endif	/* TREE_BUILDER_H */
