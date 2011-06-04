/*
 * CFG.cpp
 *
 *  Created on: Jun 3, 2011
 *      Author: ahmedkotb
 */

#include "CFG.h"

CFG::CFG(string grammar_file_path) {
	terminals = new set<string> ();
	nonterminals = new set<string> ();
	productions = new map<string, vector<vector<string>*>*> ();
	parse_file(grammar_file_path);
}

void CFG::parse_file(string path) {
	Lexical_Analyser lex(GRAMMAR_FILE_PATH, path);

	Token * tok = lex.get_next_token();
	bool first_symbol_passed = false;
	bool equal_passed = false;
	string head;
	vector<string>* current_production;

	while (true) {
		//end of file
		if (tok == NULL)
			break;
		//new production
		if (tok->get_token_type() != HASH) {
			//illegal start of production
			cout << "illegal start of production";
			throw -1;
		}

		equal_passed = false;
		head = "";
		while (true) {
			delete tok;
			tok = lex.get_next_token();
			if (tok == NULL) break;
			//cout << tok->get_token_lexeme()  << "  " << tok->get_token_type()  << endl;
			if (tok->get_token_type() == NON_TERMINAL) {
				//first symbol in grammar
				if (first_symbol_passed == false) {
					start_symbol = tok->get_token_lexeme();
					head = tok->get_token_lexeme();
					nonterminals->insert(head);
					first_symbol_passed = true;
				} else if (equal_passed == false) {
					if (head == "") {
						head = tok->get_token_lexeme();
						nonterminals->insert(head);
					} else {
						//ERROR another non terminal before =
						cout << "ERROR another non terminal before =";
						throw -1;
					}
				} else if (equal_passed == true) {
					current_production->push_back(tok->get_token_lexeme());
				}
			} else if (tok->get_token_type() == EQUAL) {
				if (equal_passed == false) {
					equal_passed = true;

					//new symbol in head
					if (productions->find(head) == productions->end())
						(*productions)[head] = new vector<vector<string>*> ();

					//init new production
					current_production = new vector<string> ();
					(*productions)[head]->push_back(current_production);

				} else {
					//ERROR more than equal in production
					cout << "ERROR more than equal in production";
					throw -1;
				}
			} else if (tok->get_token_type() == OR_CHAR) {
				//init new production
				current_production = new vector<string> ();
				(*productions)[head]->push_back(current_production);
			} else if (tok->get_token_type() == TERMINAL) {
				if (equal_passed == false){
					cout << "terminal in LHS of =";
					throw -1;
				}
				terminals->insert(tok->get_token_lexeme().substr(1,tok->get_token_lexeme().length()-2));
				current_production->push_back(tok->get_token_lexeme().substr(1,tok->get_token_lexeme().length()-2));
			}else if (tok->get_token_type() == EPSILON_CHAR){
				if (equal_passed == false){
					cout << "Epsilon in LHS of =";
					throw -1;
				}
				if (current_production->size() > 0){
					cout << "epsilon character is not alone in production";
					throw -1;
				}
				current_production->push_back(EPSILON_TOKEN);
				//TODO fix this
				//skip next production which will be L
				tok = lex.get_next_token();
			} else {
				break;
			}
		}
	}
}

string CFG::get_start_symbol(){
	return start_symbol;
}

set<string>* CFG::get_terminals(){
	return terminals;
}

set<string>* CFG::get_nonterminals(){
	return nonterminals;
}

map<string,vector<vector<string>*>*>* CFG::get_productions(){
	return productions;
}

void CFG::debug(){
	cout << "Terminals : \n===========\n";
	set<string>::iterator it;
	for (it = terminals->begin(); it != terminals->end() ; ++it){
		cout << "'" << *it << "' , ";
	}
	cout << endl;
	cout << "Non Terminals : \n===============\n";
	for (it = nonterminals->begin(); it != nonterminals->end() ; ++it){
		cout << *it << " , ";
	}
	cout << endl;
	cout << "Productions :\n=============\n";
	map<string,vector<vector<string>*>*>::iterator map_it;

	for (map_it = productions->begin() ; map_it != productions->end() ; ++map_it){

		vector<vector<string>*>* productions_of_head = map_it->second;
		cout << map_it->first << " ==> ";
		for (unsigned int i=0; i< productions_of_head->size() ;++i){
			if (i != 0 && i!= productions_of_head->size())
				cout << " | ";
			vector<string>* tokens = productions_of_head->at(i);
			for (unsigned int j = 0; j < tokens->size(); ++j) {
				if (terminals->find(tokens->at(j)) == terminals->end())
					cout << tokens->at(j) << " ";
				else
					cout << "'" << tokens->at(j) << "' ";
			}
		}
		cout << endl;
	}

}
CFG::~CFG() {
	delete terminals;
	delete nonterminals;
	delete productions;
}
