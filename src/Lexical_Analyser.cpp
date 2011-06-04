/*
 * Lexical_Analyser.cpp
 *
 *  Created on: May 27, 2011
 *      Author: ahmedkotb
 */

#include "Lexical_Analyser.h"

Lexical_Analyser::Lexical_Analyser(string input_file_path,
		string source_file_path) {
	//parse the input file and get the DFA
	Parser* p = new Parser();
	Parsing_output* output = p->parse(input_file_path);
	vector<string>::iterator it;

	//store token names
	token_names = output->get_lan_tokens();

	//tokens
//	cout << "Tokens :" << endl;
//	for (it = token_names->begin(); it != token_names->end(); it++)
//		cout << (*it) << endl;

	//nfa
	NFA* nfa = output->get_combinedNFA();
	//dfa
	dfa = new DFA(nfa);
	dfa->minimize();
	//--------------------------------------
	//allocate memory for both buffers
	buffer1 = new char[BUFFER_SIZE];
	buffer2 = new char[BUFFER_SIZE];
	//fill buffer1
	//init vars
	lexeme_begin = buffer1;
	forward = buffer1;
	lexeme_begin_buffer_num = 1;
	forward_buffer_num = 1;
	//open stream
	is.open(source_file_path.c_str(), ios::binary);

	fill_buffer(buffer1);
}

void Lexical_Analyser::fill_buffer(char * buffer) {
	is.read(buffer, BUFFER_SIZE);
	//if eof reached file ended
	if (is.eof())
		buffer[is.gcount()] = '\0';
}

string Lexical_Analyser::get_lexeme(char *start, char *end) {
	char *p = start;
	string lex = "";
	if (lexeme_begin_buffer_num == forward_buffer_num) {
		while (p != end) {
			lex += *p++;
		}
	} else {
		p = start;
		for (int i = lexeme_begin_counter; i < BUFFER_SIZE; ++i) {
			lex += *p++;
		}
		p = buffer2;
		while (p != end) {
			lex += *p++;
		}
	}
	return lex;
}

Token * Lexical_Analyser::get_next_token() {
	static bool update = true;
	string lexeme, token_name;

	NFA_State *state = dfa->get_start_state();
	int last_accepting = -1;

	Token * ret_token = NULL;
	while (true) {
		//reached end of file
		if (*forward == '\0') {
			//TODO handle error
			return NULL;
		}
		//longest matched token
		if (state ->is_accepting_state())
			last_accepting = state->get_token_id();

		char input_char = *forward;
		state = state ->get_dfa_transition(input_char);

		//check dead state
		if (state == NULL) {
			//reset lexeme and token name
			lexeme = "";
			token_name = "";

			if (last_accepting == -1) {
				//Exception
				//skip the corrupted token
				//recover
				if (forward == lexeme_begin) {
					forward++;
					lexeme_begin++;
					forward_counter++;
				} else {
					lexeme_begin = forward;
				}
			} else {
				lexeme = get_lexeme(lexeme_begin, forward);
				token_name = token_names->at(last_accepting);
				if (token_name.compare("White Space") != 0) {
					ret_token = new Token(lexeme, token_name);
				} else {
					//TODO fix this part
					assert(last_accepting == 0);
				}
				lexeme_begin = forward;
			}
			last_accepting = -1;
			state = dfa->get_start_state();
			//lexeme_begin = forward;
			lexeme_begin_buffer_num = forward_buffer_num;
			lexeme_begin_counter = forward_counter;
			update = false;
		}
		if (update) {
			forward++; //update pointer
			forward_counter++;
		} else {
			update = true;
		}
		//handle buffers
		if (forward_counter == BUFFER_SIZE && forward_buffer_num == 1) {
			//at end buffer one
			fill_buffer(buffer2);
			forward = buffer2;
			forward_buffer_num = 2;
			forward_counter = 0;
		} else if (forward_counter == BUFFER_SIZE && forward_buffer_num == 2) {
			//at end buffer two
			fill_buffer(buffer1);
			forward = buffer1;
			forward_buffer_num = 1;
			forward_counter = 0;
		}
		if (ret_token != NULL)
			return ret_token;
	}
	return ret_token;
}
Lexical_Analyser::~Lexical_Analyser() {
	delete token_names;
	is.close();
}
