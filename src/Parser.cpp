//============================================================================
// Name        : Phase.cpp
// Author      : Ahmed elbagoury
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include "Parser.h"
using namespace std;
string operators[OPERATOR_NUM] = { "*", "+", "|", " " };
//The specail chars
char special_char[SPECIAL_NUM] = { '{', '}', '[', ']', '|', '=', ':', '+', '*',
		'(', ')' };
//The tokens of the input line

Parser::Parser(vector<string> in) {
	input = in;
}

Parser::Parser() {
}

Parser::~Parser() {
}

void Parser::gotOper(string opThis, int prec1) {
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

void Parser::gotParen(string ch) {
	while (!op_Stack.empty()) {
		string chx = op_Stack.top();
		op_Stack.pop();
		if (chx == "(")
			break;
		else
			output.push_back(chx);
	}
}

void Parser::scan() {

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

	//cout << output[0] << endl;
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
	NFA *x = stringToNFA(&output);
	x->debug();
}

bool Parser::isOperator(string token) {
	for (int i = 0; i < OPERATOR_NUM; i++) {
		if (token == operators[i])
			return true;
	}
	return false;
}

NFA * Parser::createByBaseCase(string str) {
	//TODO to determine if it's a sequence or range
	NFA * ret;
	if (str[1] == '-')
		ret = new NFA((INPUT_CHAR) str[0], (INPUT_CHAR) str[2]);
	else
		ret = new NFA(str);
	return ret;
}

NFA* Parser::stringToNFA(vector<string> *in) {
	for (unsigned int i = 0; i < in->size(); i++) {
		string cur = in->at(i);
		if (!isOperator(cur)) //This is an operand
		{
			map<string, vector<string> >::iterator it = RDMap.find(cur);
			if (it == RDMap.end()) //This is a sequence or Range .....Base case
			{
				NFA *nfa = createByBaseCase(cur);
				operand_stack.push(nfa);
			} else //This is a regular definition
			{
				NFA *nfa = stringToNFA(&it->second);
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
				NFA * result = operand_stack.top();
				operand_stack.pop();
				result->apply_star_closure();
				operand_stack.push(result);
			} else //Plus closure
			{
				NFA *result = operand_stack.top();
				operand_stack.pop();
				result->apply_plus_closure();
				operand_stack.push(result);
			}
			//Binary operation
			if (operation == NFA_JOIN_OR || operation == NFA_JOIN_CONCATENATE) {
				//Get the first element
				NFA * nfa1 = operand_stack.top();
				operand_stack.pop();
				cout << "here " << operand_stack.size() << endl;
				//Get the second element
				NFA * nfa2 = operand_stack.top();
				operand_stack.pop();
				cout << "here " << operand_stack.size() << endl;
				//Join
				nfa1->join(nfa2, operation);
				operand_stack.push(nfa1);
			}
		}
	}

	assert(operand_stack.size() == 1);
	NFA * result = operand_stack.top();
	operand_stack.pop();
	return result;
}

bool Parser::isSpecialChar(char c) {
	for (int i = 0; i < SPECIAL_NUM; i++) {
		if (c == special_char[i])
			return true;
	}
	return false;
}

void Parser::split(string line) {
	//if we read a space
	int st = 0;
	int end = -1;
	bool look_back = false; //To construct a token from the previous chars
	char cur_char;
	char lastChar;
	bool isRHS = false; //To indicate if the RHS is proccessed now or not yet
	bool lastIsSpace = false;
	string str;
	vector<string> tokens;
	for (unsigned int i = 0; i < line.length(); i++) {
		cur_char = line[i];
		if (isSpecialChar(cur_char)) //current char is special char and is inserted directly
		{
			lastIsSpace = false;
			if (end >= st && look_back) //Storing the accumulated string
			{
				tokens.push_back(line.substr(st, end - st + 1));
				st = end + 1;
				look_back = false;
			}

			//To handle concatination
			if (cur_char == '(' && isRHS) {
				if (!isSpecialChar(lastChar) || lastChar == '+' || lastChar
						== '*' || lastChar == ')')
					tokens.push_back(CONCATINATION_SYMBOL);
			}
			str = cur_char;
			tokens.push_back(str);
			st++;
			end++;
			if (str == "=" || str == ":")
				isRHS = true;
			lastChar = cur_char;
		} else if (cur_char == '\\') //Current char is \ meaning that it's followed by a special character
		{
			if (end >= st && look_back) //Storing the accumulated string
			{
				tokens.push_back(line.substr(st, end - st + 1));
				st = end + 1;
				look_back = false;
			}
			if (isRHS) {
				if (!isSpecialChar(lastChar) || lastChar == '+' || lastChar
						== '*' || lastChar == ')')
					tokens.push_back(CONCATINATION_SYMBOL);
			}
			str = cur_char + line[++i];
			tokens.push_back(str); //pushing the current character and the next one
			st += 2;
			end += 2;
			lastChar = '\\' + line[i];
			lastIsSpace = false;
		} else if (cur_char == ' ' || cur_char == '\t') {
			lastIsSpace = true;
			if (end >= st && look_back) {
				tokens.push_back(line.substr(st, end - st + 1));
				st = end + 2;
				end++;
				look_back = false;
			} else {
				st++;
				end++;
			}
		} else {
			end++;
			look_back = true;
			if (isSpecialChar(lastChar)) {
				if (lastChar == '+' || lastChar == '*' || lastChar == ')')
					tokens.push_back(" ");
			} else //The last character is not a special character
			{
				if (lastIsSpace && isRHS)
					tokens.push_back(" ");
			}

			if (i == line.length() - 1) {
				tokens.push_back(line.substr(st, end - st + 1));
			}
			lastChar = cur_char;
			lastIsSpace = false;
		}
	}
	input = tokens;
	scan();
}

//int main() {
//	string line;
//	Parser p;
//	ifstream in_file("input file.txt");
//	if (in_file.is_open()) {
//		while (!in_file.eof()) {
//			getline(in_file, line);
//			cout << line << endl;
//			if (line == "")
//				break;
//			p.split(line);
//		}
//		in_file.close();
//	} else
//		cout << "Unable to open file";
//	cout << "----------------------" << endl;
//
//	return 0;
//}
