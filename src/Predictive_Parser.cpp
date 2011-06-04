/*
 * Predictive_Parser.cpp
 *
 *  Created on: Jun 3, 2011
 *      Author: ahmedkotb
 */

#include "Predictive_Parser.h"

Predictive_Parser::Predictive_Parser(CFG *grammar) {
	this->grammar = grammar;
	first = new map<string, set<string>*> ();
	follow = new map<string, set<string>*> ();

	//calculate first
	set<string>* nonterminals = grammar->get_nonterminals();
	for (set<string>::iterator it = nonterminals->begin(); it
			!= nonterminals->end(); ++it) {
		calc_first(*it);
	}

	calc_follow();
}

void Predictive_Parser::calc_first(string head) {
	vector<vector<string>*> * productions =
			(*(grammar->get_productions()))[head];

	//if already calculated then return
	if (first->find(head) != first->end())
		return;

	//init set
	set<string> * production_first = new set<string> ();
	(*first)[head] = production_first;

	//terminal added from this production
	int added;
	//loop on productions
	for (unsigned int i = 0; i < productions->size(); ++i) {

		added = 0;
		//loop on tokens of specific production
		for (unsigned int j = 0; j < productions->at(i)->size(); ++j) {
			string token = productions->at(i)->at(j);
			//if terminal
			if (grammar->get_terminals()->find(token)
					!= grammar->get_terminals()->end()) {
				++added;
				production_first->insert(token);
				break;
			} else {
				//NON terminal
				if (token == EPSILON_TOKEN)
					continue;
				//wasn't calculated before
				if (first->find(token) == first->end()) {
					calc_first(token);
				}

				bool has_epsilon = false;
				set<string> * token_first = first->find(token)->second;
				set<string>::iterator it = token_first->begin();
				for (; it != token_first->end(); ++it) {
					if ((*it) == EPSILON_TOKEN)
						has_epsilon = true;
					else {
						++added;
						production_first->insert(*it);
					}
				}
				//if first of token doesn't contain epsilon then
				//no need to see the rest of this production
				if (!has_epsilon)
					break;
			}
		}
		//add epsilon to first of this non terminal
		if (added == 0)
			production_first->insert(EPSILON_TOKEN);

	}

}

void Predictive_Parser::calc_follow() {
	set<string> work_set;
	queue<string> work_queue;
	map<string, set<string>*> constraints;
	//phase 1 : calc follow using rule 1 and 2
	set<string>* nonterminals = grammar->get_nonterminals();

	//loop on non-terminals
	for (set<string>::iterator non_terminal_it = nonterminals->begin(); non_terminal_it
			!= nonterminals->end(); ++non_terminal_it) {

		string nonterminal = *(non_terminal_it);
		//init follow set
		(*follow)[nonterminal] = new set<string> ();

		//add end marker to start symbol
		if (nonterminal == grammar->get_start_symbol())
			follow->find(nonterminal)->second->insert(INPUT_END_MARKER);


		map<string, vector<vector<string>*>*>::iterator prod_it =
				grammar->get_productions()->begin();
		//loop on all productions
		for (; prod_it != grammar->get_productions()->end(); ++prod_it) {
			vector<vector<string>*> * productions =
					(*(grammar->get_productions()))[prod_it->first];

			//loop on this production
			for (unsigned int prod_i = 0; prod_i < productions->size(); ++prod_i) {
				vector<string>*tokens = productions->at(prod_i);
				//loop on tokens
				for (unsigned int tok_i = 0; tok_i < tokens->size(); ++tok_i) {
					if (tokens->at(tok_i) != nonterminal)
						continue;
					//loop on next tokens
					unsigned int tok_j = tok_i + 1;
					for (; tok_j < tokens->size(); ++tok_j) {
						//token is terminal
						if (grammar->get_terminals()->find(tokens->at(tok_j))
								!= grammar->get_terminals()->end()) {
							follow->find(nonterminal)->second->insert(tokens->at(tok_j));
							tok_i = tok_j;
							break;
						} else {
							//token is nonterminal , add its first to our follow
							set<string> * token_first = first->find(tokens->at(tok_j))->second;
							set<string>::iterator it = token_first->begin();
							bool has_epsilon = false;
							for (; it != token_first->end(); ++it) {
								if ((*it) == EPSILON_TOKEN)
									has_epsilon = true;
								else
									follow->find(nonterminal)->second->insert(
											*it);
							}
							if (!has_epsilon) {
								tok_i = tok_j;
								break;
							}
						}
					}
					if (tok_j >= tokens->size()) {
						//reached end of production
						if (constraints.find(prod_it->first) == constraints.end())
							constraints[prod_it->first] = new set<string>();
						constraints[prod_it->first]->insert(nonterminal);
					}
				}
			}

		}
	}
	//-----------------------------------------------------------------------------
	//phase 2
	//add all nonterminals on constraints map
	map<string,set<string>*>::iterator constr_it = constraints.begin();
	while (constr_it != constraints.end()){
		work_queue.push(constr_it->first);
		work_set.insert(constr_it->first);
		++constr_it;
	}
	while (work_queue.size() > 0){
		string non_termainl = work_queue.front();
		work_queue.pop();
		work_set.erase(non_termainl);
		set<string> * affected_nonterminals = constraints[non_termainl];
		set<string>* affecting_follow = follow->find(non_termainl)->second;
		set<string>::iterator it = affected_nonterminals->begin();
		while (it != affected_nonterminals->end()){
			set<string>* affected_follow = follow->find(*it)->second;
			unsigned int size_before = affected_follow->size();
			set<string>::iterator affecting_follow_it = affecting_follow->begin();
			while (affecting_follow_it != affecting_follow->end()){
				affected_follow->insert(*affecting_follow_it);
				++affecting_follow_it;
			}
			if (affected_follow->size() != size_before){
				//follow of affected nonterminal is changed
				//if terminal is not already in queue and it is from the affecting terminals
				if (work_set.find(*it) == work_set.end() && constraints.find(*it) != constraints.end()){
					work_queue.push(*it);
				}
			}
			++it;
		}
	}

	//free memory
	constr_it = constraints.begin();
	while (constr_it != constraints.end()){
		delete constr_it->second;
		++constr_it;
	}
}

void Predictive_Parser::debug() {
	map<string, set<string>*>::iterator it;
	cout << "FIRST SETS :\n===========\n";
	for (it = first->begin(); it != first->end(); ++it) {
		cout << it->first << " : ";
		set<string> * first_set = it->second;
		assert (first_set != NULL);
		for (set<string>::iterator its = first_set->begin(); its
				!= first_set->end(); ++its) {
			cout << *its << " , ";
		}
		cout << endl;
	}
	cout << "FOLLOW SETS :\n===========\n";
	for (it = follow->begin(); it != follow->end(); ++it) {
		cout << it->first << " : ";
		set<string> * follow_set = it->second;
		assert (follow_set != NULL);
		for (set<string>::iterator its = follow_set->begin(); its
				!= follow_set->end(); ++its) {
			cout << *its << " , ";
		}
		cout << endl;
	}
}
Predictive_Parser::~Predictive_Parser() {
	// TODO Auto-generated destructor stub
}
