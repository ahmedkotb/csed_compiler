/*
 * Predictive_Parser.cpp
 *
 *  Created on: Jun 3, 2011
 *      Author: ahmedkotb
 */

#include "Predictive_Parser.h"

Predictive_Parser::Predictive_Parser(CFG *grammar) {

	//init vars
	this->grammar = grammar;
	first = new map<string, set<string>*> ();
	follow = new map<string, set<string>*> ();
	subset_first = new map<pair<string,int>,set<string>*>();

	parse_table = new Parse_Table_Entry*[grammar->get_nonterminals()->size()];

	//size = size of terminals + 1 (input end marker)
	for (unsigned int i = 0; i < grammar->get_nonterminals()->size(); ++i)
		parse_table[i] = new Parse_Table_Entry[grammar->get_terminals()->size() + 1];


	//calculate first
	set<string>* nonterminals = grammar->get_nonterminals();
	for (set<string>::iterator it = nonterminals->begin(); it
			!= nonterminals->end(); ++it) {
		calc_first(*it);
	}

	//calculate follow
	calc_follow();

	//generate parsing table
	generate_table();
}

void Predictive_Parser::calc_first(string head) {
	//if already calculated then return
	if (first->find(head) != first->end())
		return;

	vector<vector<string>*> * productions =
			(*(grammar->get_productions()))[head];

	//init set
	set<string> * production_first = new set<string> ();
	(*first)[head] = production_first;

	//terminal added from this production
	int added;
	//loop on productions
	for (unsigned int i = 0; i < productions->size(); ++i) {
		//init subset of this production
		(*subset_first)[make_pair(head,i)] = new set<string>();

		added = 0;
		//loop on tokens of specific production
		for (unsigned int j = 0; j < productions->at(i)->size(); ++j) {
			string token = productions->at(i)->at(j);
			//if terminal
			if (grammar->get_terminals()->find(token)
					!= grammar->get_terminals()->end()) {
				++added;
				production_first->insert(token);
				//add to subset first of this production
				subset_first->find(make_pair(head,i))->second->insert(token);
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
						//add to subset first of this production
						subset_first->find(make_pair(head,i))->second->insert(*it);
					}
				}
				//if first of token doesn't contain epsilon then
				//no need to see the rest of this production
				if (!has_epsilon)
					break;
			}
		}
		//add epsilon to first of this non terminal
		if (added == 0){
			production_first->insert(EPSILON_TOKEN);
			//add to subset first of this production
			subset_first->find(make_pair(head,i))->second->insert(EPSILON_TOKEN);
		}

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

void Predictive_Parser::generate_table(){
	//numbering terminals
	int index = 0;
	terminals_num[INPUT_END_MARKER] = index;
	set<string>::iterator it = grammar->get_terminals()->begin();
	while (it != grammar->get_terminals()->end()){
		terminals_num[*it] = ++index;
		++it;
	}
	//numbering non terminals
	index = -1;
	it = grammar->get_nonterminals()->begin();
	while (it != grammar->get_nonterminals()->end()){
		non_terminals_num[*it] = ++index;
		++it;
	}

	//assign table entries
	//loop on all productions
	map<string,vector<vector<string>*>*>::iterator prod_it = grammar->get_productions()->begin();
	while (prod_it != grammar->get_productions()->end()){
		vector<vector<string>*> * productions = prod_it->second;
		int head_no = non_terminals_num[prod_it->first];

		bool made_epsilon_transitions = false;
		//loop on all productions of given head
		for (unsigned int i= 0;i<productions->size();++i){
			//loop on all first of this production
			set<string> * first_sub_set = subset_first->find(make_pair(prod_it->first,i))->second;
			set<string>::iterator first_it = first_sub_set->begin();

			bool has_epsilon = false;
			while (first_it != first_sub_set->end()){
				if (*first_it == EPSILON_TOKEN) has_epsilon = true;
				int terminal_no = terminals_num[*first_it];
				parse_table[head_no][terminal_no].set_production_num(i);
				++first_it;
			}

			if (has_epsilon && !made_epsilon_transitions){
				set<string> * follow_set = follow->find(prod_it->first)->second;
				set<string>::iterator follow_set_it = follow_set->begin();
				while (follow_set_it != follow_set->end()){
					int terminal_no = terminals_num[*follow_set_it];
					parse_table[head_no][terminal_no].set_epsilon(true);
					++follow_set_it;
				}
				made_epsilon_transitions = true;
			}
			if (has_epsilon){
				//loop on follow set of this head production and if entry is not epsilon entry then make it sync
				set<string> * follow_set = follow->find(prod_it->first)->second;
				set<string>::iterator follow_set_it = follow_set->begin();
				while (follow_set_it != follow_set->end()){
					int terminal_no = terminals_num[*follow_set_it];
					assert(parse_table[head_no][terminal_no].has_entry() == false);
					if (!parse_table[head_no][terminal_no].is_epsilon())
						parse_table[head_no][terminal_no].set_sync(true);
					++follow_set_it;
				}
			}
		}

		++prod_it;
	}
}

void Predictive_Parser::parse(string rules_file_path , string source_file_path){
	Lexical_Analyser lex (rules_file_path,source_file_path);
	Token* current_token = lex.get_next_token();
	list<string> stk;
	list<string> lst;
	stk.push_back(INPUT_END_MARKER);
	stk.push_back(grammar->get_start_symbol());
	string token = current_token->get_token_type();
	cout << grammar->get_start_symbol() << endl;
	bool match = false;

	bool production_print = false;


	while (!stk.empty()){
		match = false;
		if (stk.back() == token){
			//both top of stack and current token match
			if (production_print)
				cout << "match " << stk.back();
			lst.push_back(token);
			delete current_token;
			current_token = lex.get_next_token();
			token = current_token == NULL  ? INPUT_END_MARKER : current_token->get_token_type();
			stk.pop_back();
			match = true;
		}else if (grammar->get_terminals()->find(stk.back()) != grammar->get_terminals()->end()){
			//terminal at top of stack doesn't match current token
			cout << "ERROR >> advance input --" << endl;
			delete current_token;
			current_token = lex.get_next_token();
			token = current_token == NULL  ? INPUT_END_MARKER : current_token->get_token_type();
		}else{
			//nonterminal at top of stack
			int non_terminal_no = non_terminals_num[stk.back()];
			int terminal_no = terminals_num[token];
			if (parse_table[non_terminal_no][terminal_no].has_entry()){
				vector<string>* production = grammar->get_productions()->find(stk.back())
						->second->at(parse_table[non_terminal_no][terminal_no].get_production_num());

				if (production_print){
					cout << stk.back() << " ==> " ;
					for (unsigned int i = 0; i < production->size(); ++i)
						cout << production->at(i) << " ";
				}
				stk.pop_back();
				//push new production in reverse order
				for (int i=production->size()-1;i >=0;--i)
					stk.push_back(production->at(i));

			}else if (parse_table[non_terminal_no][terminal_no].is_epsilon()){
				if (production_print)
					cout << stk.back() << " ==> " << EPSILON_TOKEN;
				stk.pop_back();
			}else if (parse_table[non_terminal_no][terminal_no].is_sync()){
				//ERROR
				//handle special case
				if (stk.size() > 2){
					cout << "SYNC ERROR" << endl;
					stk.pop_back();
				}else{
					cout << "ERROR >> advance input" << endl;
					delete current_token;
					current_token = lex.get_next_token();
					token = current_token == NULL  ? INPUT_END_MARKER : current_token->get_token_type();
				}
			}else{
				//ERROR entry
				cout << "ERROR >> advance input";
				delete current_token;
				current_token = lex.get_next_token();
				token = current_token == NULL  ? INPUT_END_MARKER : current_token->get_token_type();
			}
		}
		if (production_print)
			cout << endl;

		if (!match){
			list<string>::iterator lst_it = lst.begin();
			while (lst_it != lst.end()){
				cout << *lst_it << " ";
				++lst_it;
			}
			list<string>::reverse_iterator stk_it = stk.rbegin();
			while (stk_it != stk.rend()){
				if (*stk_it == INPUT_END_MARKER) break;
				cout << *stk_it << " ";
				++stk_it;
			}
			cout << endl;
		}
	}
	cout << "------------------------------------------" << endl;
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
//		cout << it->first << " SUBSETS : ";
//		int num_of_productions = grammar->get_productions()->find(it->first)->second->size();
//		for (int i = 0; i < num_of_productions; ++i) {
//			set<string>::iterator subset_it = subset_first->find(make_pair(it->first,i))->second->begin();
//			while (subset_it != subset_first->find(make_pair(it->first,i))->second->end()){
//				cout << *subset_it << " , ";
//				++subset_it;
//			}
//			if (i != num_of_productions-1)
//				cout << " | ";
//		}
//		cout << endl;
	}
	cout << "==========" <<endl;
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
	cout << "==========" <<endl;
	cout << "PARSING TABLE :\n==============\n";
	//loop on nonterminals
	set<string>::iterator non_terminals_it = grammar->get_nonterminals()->begin();
	while (non_terminals_it != grammar->get_nonterminals()->end()){
		int non_terminal_no = non_terminals_num[*non_terminals_it];
		//loop on terminals
		set<string>::iterator terminals_it = grammar->get_terminals()->begin();
		while (terminals_it != grammar->get_terminals()->end()){
			int terminal_no = terminals_num[*terminals_it];
			if (parse_table[non_terminal_no][terminal_no].has_entry()){
				vector<string>* p = grammar->get_productions()->find(*non_terminals_it)
					   ->second->at(parse_table[non_terminal_no][terminal_no].get_production_num());
				string production_string ="";
				for (unsigned int i=0;i<p->size();++i)
					production_string += p->at(i) + " ";

				cout << "( " << *non_terminals_it << " , " << *terminals_it << " ) ==> " << production_string << endl;
			}else if (parse_table[non_terminal_no][terminal_no].is_epsilon()){
				cout << "( " << *non_terminals_it << " , " << *terminals_it << " ) ==> " << EPSILON_TOKEN << endl;
			}else if (parse_table[non_terminal_no][terminal_no].is_sync()){
				cout << "( " << *non_terminals_it << " , " << *terminals_it << " ) ==> " << "#SYNC#" << endl;
			}
			++terminals_it;
		}
		// test INPUT END MARKER
		if (parse_table[non_terminal_no][0].is_epsilon()){
			cout << "( " << *non_terminals_it << " , " << "#$#" << " ) ==> " << EPSILON_TOKEN << endl;
		}else if (parse_table[non_terminal_no][0].is_sync()){
			cout << "( " << *non_terminals_it << " , " << "#$#" << " ) ==> " << "#SYNC#" << endl;
		}
		++non_terminals_it;
	}
}

Predictive_Parser::~Predictive_Parser() {
	map<string,set<string>*>::iterator f_it= first->begin();
	while (f_it != first->end()){
		delete f_it->second;
		++f_it;
	}
	delete first;

	f_it = follow->begin();
	while (f_it != follow->end()){
		delete f_it->second;
		++f_it;
	}
	delete follow;
	for(unsigned int i=0;i<grammar->get_nonterminals()->size();++i)
		delete[] parse_table[i];
	delete parse_table;
	//loop on nonterminals
	set<string>::iterator non_terminals_it = grammar->get_nonterminals()->begin();
	while (non_terminals_it != grammar->get_nonterminals()->end()){
		vector<vector<string>*> * productions = (*grammar->get_productions())[*non_terminals_it];
		for (unsigned int i = 0; i < productions->size(); ++i)
			delete subset_first->find(make_pair(*non_terminals_it,i))->second;
		++non_terminals_it;
	}
	delete subset_first;

}
