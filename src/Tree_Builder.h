/* 
 * File:   Tree_Builder.h
 * Author: 3mara
 *
 * Created on April 21, 2011, 11:26 PM
 */

#ifndef TREE_BUILDER_H
#define	TREE_BUILDER_H


#include <iostream>
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
using namespace std;

class Tree_Builder {
public:
    stack <string> op_Stack;
    vector <string>output;
    vector <string>input;
    Tree_Builder(vector <string> in);
    Tree_Builder();
    virtual ~Tree_Builder();
    void gotOper(string opThis, int prec1);
    void gotParen(string ch);
    void scan();
    bool isOperator(string token);
    NFA createByBaseCase(string str);
    NFA stringToNFA(vector <string> *in);
private:
    map<string, vector<string> > keyWordMap;
    map<string, vector<string> > RDMap;
    stack <NFA> operand_stack;

};

#endif	/* TREE_BUILDER_H */
