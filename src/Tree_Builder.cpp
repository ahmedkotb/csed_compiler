/* 
 * File:   Tree_Builder.cpp
 * Author: 3mara
 * 
 * Created on April 21, 2011, 11:26 PM
 */
#include "Tree_Builder.h"
#define OPERATOR_NUM 4
using namespace std;

string operators[OPERATOR_NUM] = {"*", "+", "|", " "};

Tree_Builder::Tree_Builder(vector<string> in) {
    input = in;
}

Tree_Builder::Tree_Builder() {
}

Tree_Builder::~Tree_Builder() {
}

void Tree_Builder::gotOper(string opThis, int prec1) {
    while (!op_Stack.empty()) {
        string opTop = op_Stack.top();
        op_Stack.pop();
        if (opTop == "(") {
            op_Stack.push(opTop);
            break;
        } else {
            int prec2;
            if (opTop == "|") {
                prec2 = 1;
            } else if (opTop == " ") {
                prec2 = 2;
            } else {
                prec2 = 3;
            }
            if (prec2 < prec1) {
                op_Stack.push(opTop);
                break;
            } else
                output.push_back(opTop);
        }
    }
    op_Stack.push(opThis);
}

void Tree_Builder::gotParen(string ch) {
    while (!op_Stack.empty()) {
        string chx = op_Stack.top();
        op_Stack.pop();
        if (chx == "(")
            break;
        else
            output.push_back(chx);
    }
}

void Tree_Builder::scan() {

    if (input[0] == "{" || input[1] == "[") {
        for (int j = 2; j < input.size(); j++) {
            keyWordMap[input[j]] = input[j];
        }
    }
    int type = 0;
    if (input[1] == ":")
        type = RE;
    else
        type = RD;
    for (int j = 2; j < input.size(); j++) {
        string ch = input.at(j);
        if (ch == " ") {
            gotOper(ch, 2);
        } else if (ch == "*" || ch == "+") {
            gotOper(ch, 3); // go pop operators
        } else if (ch == "|") {
            gotOper(ch, 1); // go pop operators
        } else if (ch == "(") {
            op_Stack.push(ch); // push it
        } else if (ch == ")") {
            gotParen(ch); // go pop operators
        } else { // write it to output
            output.push_back(ch);
        }
    }
    while (!op_Stack.empty()) {
        output.push_back(op_Stack.top());
        op_Stack.pop();
    }

    cout << output[0] << endl;
    switch (type) {
        case RE:
            stringToNFA(&output);
            break;
        default: //Insert the RD in the RDMap
        {
            string rd = input[0];
            RDMap[rd] = output;
            break;
        }
    }
}

bool Tree_Builder::isOperator(string token) {
    for (int i = 0; i < OPERATOR_NUM; i++) {
        if (token == operators[i])
            return true;
    }
    return false;
}

NFA Tree_Builder::createByBaseCase(string str) {
    //TODO to determine if it's a sequence or range
    if (str[1] == '-')
        return NFA((INPUT_CHAR) str[0], (INPUT_CHAR) str[2]);
    else
        return NFA(str);

}

NFA Tree_Builder::stringToNFA(vector<string> *in) {

    for (int i = 0; i < in->size(); i++) {
        string cur = in->at(i);
        if (!isOperator(cur)) //This is an operand
        {
            map<string, vector<string> >::iterator it = RDMap.find(cur);
            if (it == RDMap.end()) //This is a sequence or Range .....Base case
            {
                NFA nfa = createByBaseCase(cur);
                operand_stack.push(nfa);
            } else //This is a regular definition
            {
                NFA nfa = stringToNFA(&it->second);
                operand_stack.push(nfa);
            }
        } else //This is an operator
        {
            int operation = -1;
            if (cur == OR)
                operation = NFA_JOIN_OR;
            else if (cur == CONCATINATE)
                operation = NFA_JOIN_CONCATENATE;
            else if (cur == CLOSURE) {
                NFA result = operand_stack.top();
                operand_stack.pop();
                result.apply_star_closure();
                operand_stack.push(result);
            } else //Plus closure
            {
                NFA result = operand_stack.top();
                operand_stack.pop();
                result.apply_plus_closure();
                operand_stack.push(result);
            }

            //Binary operation
            if (operation == NFA_JOIN_OR || operation == NFA_JOIN_CONCATENATE) {
                //Get the first element
                NFA nfa1 = operand_stack.top();
                operand_stack.pop();
                //Get the second element
                NFA nfa2 = operand_stack.top();
                operand_stack.pop();
                //Join
                nfa1.join(&nfa2, operation);
                operand_stack.push(nfa1);
            }
        }
    }
    return NULL;
}

