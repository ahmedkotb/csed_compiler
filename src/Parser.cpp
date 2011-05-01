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
//The special chars
char special_char[SPECIAL_NUM] = { '{', '}', '[', ']', '|', '=', ':', '+', '*',
		'(', ')' };
//The tokens of the input line

//Parser::Parser(vector<string>* in) {
//	input = in;
//}

Parser::Parser() {
	 this->output = new vector<string>();
	 this->RDMap = new map<string,vector<string>* >();
	 this->keyWordMap = new set<string>();
	 this->punctuation = new vector<string>();
	 this->RE_names = new vector<string>();
	 this->RE_list = new vector<vector<string>*>();
	 this->lan_tokens = new vector<string>();
	 this->NFAS = new vector<NFA*>();
}

Parser::~Parser() {
	delete(this->RDMap);
	delete(this->keyWordMap);
	delete(this->output);
}

vector <NFA*>* Parser::getNFAS(){
	return this->NFAS;
}

vector<string>* Parser::getTokens(){
	return this->lan_tokens;
}

Parsing_output* Parser::parse(string file_name){
	string line;
	ifstream in_file(file_name.c_str());
	if (in_file.is_open())
	{
		while (!in_file.eof())
		{
			getline(in_file, line);
			if(line != "")
			{
				if (line == "")
				{
					break;
				}
				split(line);
			}
		}
		in_file.close();
		NFA* combined = this->addTokens();
		Parsing_output* c = new Parsing_output(combined,this->lan_tokens);
		return c;
	}

	else
	{
		cout << "Unable to open file";
		return NULL;
	}



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
				output->push_back(opTop);
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
			output->push_back(chx);
	}
}

NFA* Parser::get_post_fix(string name , vector<string>* value,bool isRD) {
	//Clear the output vector if is used previously in RD or RE
	output->clear();
	for (unsigned int j = 0; j < value->size(); j++) {
		string ch = value->at(j);
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
			output->push_back(ch);
		}
	}
	while (!op_Stack.empty()) {
		output->push_back(op_Stack.top());
		op_Stack.pop();
	}
	if(isRD)
	{
		vector<string>* temp = new vector<string>(output->begin() , output->end());
		this->RDMap->operator [](name) = temp;
		return NULL;
	}
	else{
		NFA * nfa = stringToNFA(output);
		return nfa;
	}

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
	stack <NFA*>* operand_stack = new stack<NFA*>();
	for (unsigned int i = 0; i < in->size(); i++) {
		string cur = in->at(i);
		if (!isOperator(cur)) //This is an operand
		{
			map<string, vector<string> *>::iterator it = RDMap->find(cur);
			if (it == RDMap->end()) //This is a sequence or Range .....Base case
			{
				NFA *nfa = createByBaseCase(cur);
				operand_stack->push(nfa);
			} else //This is a regular definition
			{
				NFA *nfa = stringToNFA(it->second);
				operand_stack->push(nfa);
			}
		}
		else //This is an operator
		{
			int operation = -1;
			if (cur == OR)
			{
				operation = NFA_JOIN_OR;
			}
			else if (cur == CONCATINATE)
				operation = NFA_JOIN_CONCATENATE;
			//Kleene Closure
			else if (cur == CLOSURE) {
				NFA * result = operand_stack->top();
				operand_stack->pop();
				result->apply_star_closure();
				operand_stack->push(result);
			} else //Plus closure
			{
				NFA *result = operand_stack->top();
				operand_stack->pop();
				result->apply_plus_closure();
				operand_stack->push(result);
			}
			//Binary operation
			if (operation == NFA_JOIN_OR || operation == NFA_JOIN_CONCATENATE) {
				//Get the first element
				NFA * nfa1 = operand_stack->top();
				operand_stack->pop();
				//Get the second element
				NFA * nfa2 = operand_stack->top();
				operand_stack->pop();
				//Join
				nfa2->join(nfa1, operation);
				operand_stack->push(nfa2);
			}
		}
	}
	assert(operand_stack->size() == 1);
	NFA * result = operand_stack->top();
	operand_stack->pop();
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
	vector<string>* tokens = new vector<string>();

	for (unsigned int i = 0; i < line.length(); i++) {
		cur_char = line[i];
		if (isSpecialChar(cur_char)) //current char is special char and is inserted directly
		{
			lastIsSpace = false;
			if (end >= st && look_back) //Storing the accumulated string
			{
				tokens->push_back(line.substr(st, end - st + 1));
				st = end + 1;
				look_back = false;
			}

			//To handle concatenation
			if (cur_char == '(' && isRHS) {
				if (!isSpecialChar(lastChar) || lastChar == '+' || lastChar
						== '*' || lastChar == ')')
					tokens->push_back(CONCATINATION_SYMBOL);
			}
			str = cur_char;
			tokens->push_back(str);
			st++;
			end++;
			if (str == "=" || str == ":")
				isRHS = true;
			lastChar = cur_char;
		} else if (cur_char == '\\') //Current char is \ meaning that it's followed by a special character
		{
			if (end >= st && look_back) //Storing the accumulated string
			{
				tokens->push_back(line.substr(st, end - st + 1));
				st = end + 1;
				look_back = false;
			}
			if (isRHS) {
				if (!isSpecialChar(lastChar) || lastChar == '+' || lastChar
						== '*' || lastChar == ')')
					tokens->push_back(CONCATINATION_SYMBOL);
			}
			str = cur_char + line[++i];
			tokens->push_back(str); //pushing the current character and the next one
			st += 2;
			end += 2;
			lastChar = '\\' + line[i];
			lastIsSpace = false;
		} else if (cur_char == ' ' || cur_char == '\t') {
			lastIsSpace = true;
			if (end >= st && look_back) {
				tokens->push_back(line.substr(st, end - st + 1));
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
					tokens->push_back(" ");
			} else //The last character is not a special character
			{
				if (lastIsSpace && isRHS)
					tokens->push_back(" ");
			}

			if (i == line.length() - 1) {
				tokens->push_back(line.substr(st, end - st + 1));
			}
			lastChar = cur_char;
			lastIsSpace = false;
		}
	}
	//add each line to it's appropriate list
	select_line_type(tokens);
	delete(tokens);
}

NFA* Parser::addTokens(){
		//add keywords to the id's in this language
		install_Keywords();
		//add the punctuation to the id's in the langauge
		install_punctuation();
		//add the regular expression
		string name;
		for(unsigned int i = 0 ; i < RE_names->size() ; i++)
		{
			name = RE_names->operator [](i);
			NFA * nfa = get_post_fix(name,RE_list->operator [](i) , false);
			this->lan_tokens->push_back(name);	//Add the name to the table of tokens name
			this->NFAS->push_back(nfa);			//Add the nfa to the table of nfa's
			nfa->finalize_NFA(lan_tokens->size()-1);
		}
		NFA* combined_nfa = NFA::create_combined_NFA(this->NFAS);
		return combined_nfa;
}


void Parser::install_Keywords(){
	set<string>::iterator it;
	//Iterate on the key words to construct it's NFA
	 for ( it=keyWordMap->begin() ; it != keyWordMap->end(); it++ )
	 {
		 //Create param vector to pass the pattern of this keyword(its name)
		 vector<string>* param = new vector<string>();
		 param->push_back(*it);
		 //get the nfa of the given keyword
		 NFA* nfa = get_post_fix(*it,param,false);
		 //add the nfa to the list of nfa's in this language
		 this->NFAS->push_back(nfa);
		 //add this token to lan_token vector
		 this->lan_tokens->push_back(*it);
		 //Finalizing nfa
		 nfa->finalize_NFA(lan_tokens->size()-1);
	 }
}

void Parser::install_punctuation(){
	 vector<string>::iterator it;
	 //Iterate on the punctuation to constructs it's NAF
	 for(it = punctuation->begin() ; it != punctuation->end(); it++){
		 //create nfa for each punctuation
		 NFA* nfa = createByBaseCase((*it));
		 //Add the created NFA to the NFAs
		 this->NFAS->push_back(nfa);
		 //Add the token to the language tokens
		 this->lan_tokens->push_back(*it);
		 //Finalize the NFA with its id
		 nfa->finalize_NFA(lan_tokens->size()-1);
	 }
}

void Parser::select_line_type(vector<string> * tokens){
	//check if the first element in the vector tokens is [ punctuation
	if(tokens->operator [](0) == START_PUNCTUATION)
	{
		//Add the punctuation to the punctuation vector
		for(unsigned int i = 1 ; i < tokens->size() -1 ; i++)
			punctuation->push_back(tokens->operator [](i));
		return;
	}
	//Check if the first element in the vector tokens is{ key word
	else if(tokens->operator [](0) == START_KEYWORD)
	{
		for(unsigned int i = 1 ; i < tokens->size() -1; i ++)
		{
			keyWordMap->insert(tokens->operator [](i));		//insert the keyword in the key word map
		}
	}
	//Regular definition
	else if(tokens->operator [](1) == "=")
	{
		string rd_name = tokens->operator [](0);
		vector<string>* rd = new vector<string>();
		for(unsigned int i = 2; i < tokens->size() ; i++)		//Copying the regular definition
			rd->push_back(tokens->operator [](i));
		get_post_fix(rd_name , rd , true);
	}
	//copying the regular expression
	else if(tokens->operator [](1) == ":") //RE
	{
		string re_name = tokens->operator [](0);
		vector<string>* re = new vector<string>();
		for(unsigned int i = 2 ; i < tokens->size() ; i++)		//Copy the RE value
		{
			re->push_back(tokens->operator [](i));
		}
		RE_names->push_back(re_name);
		RE_list->push_back(re);
	}
}
