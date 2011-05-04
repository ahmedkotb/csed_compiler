/*
 * Driver.cpp
 *
 *  Created on: May 3, 2011
 *      Author: ahmed
 */

#include "Driver.h"

Driver::Driver(DFA *my_dfa, string file_name, vector<string>*token_names) {
	dfa = my_dfa;
	src_file_name = file_name;
	tokens = token_names;

	lexeme_begin_buffer_num = 1;
	forward_buffer_num = 1;

	lexeme_begin_counter = 0;
	forward_counter = 0;

	//open stream
	is.open(src_file_name.c_str(), ios::binary);

	// get length of file:
	is.seekg(0, ios::end);
	total_input_size = is.tellg();
	is.seekg(0, ios::beg);

}

char* Driver::read_file() {
	//allocate memory
	char *buffer = new char[BUFFER_SIZE];

	is.read(buffer, BUFFER_SIZE);
	//if(buffer[is.gcount()] != 10)
		buffer[is.gcount()] = '\0';
	//else
		//buffer[is.gcount() -1] = '\0';

	/*char *buffer_helper = new char[BUFFER_SIZE];
	char *p = buffer;
	char *p2 = buffer_helper;
	while((int)*p != 10)
	{
		*p2++ = *p++;
	}
	*p2 = '\0';
*/
	return buffer;
}

string Driver::create_lexem(char *start, char *end) {
	char *p = start;
	string lex = "";
	if(lexeme_begin_buffer_num == forward_buffer_num)
	{
	while (p != end) {
		lex += *p++;
	}
	}
	else
	{
		p = start;
		for (int i = lexeme_begin_counter; i < BUFFER_SIZE; ++i) {
			lex += *p++;
		}
		p = buffer2;
		while(p != end)
		{
			lex += *p++;
		}
	}
	return lex;
}

string Driver::get_buffer_contents(char *buffer) {
	char *p = buffer;
	stringstream info;
	info << "[ ";

	for (int i = 0; i < BUFFER_SIZE; ++i) {
		if (i == BUFFER_SIZE - 1)
			info << *p++;
		else
			info << *p++ << " ,";
	}

	info << " ] \n";

	return info.str();
}

map<string, string> Driver::get_symbol_table() {

	return symbol_table;
}

void Driver::simulate() {
	//output buffer
	ofstream fout;
	fout.open (OUTPUT_FILE);


	int simulateDone = 0;
	bool update = true;
	string lexeme, token_name;
	//load buffers
	buffer1 = read_file();
	lexeme_begin = buffer1;
	forward = buffer1;
	NFA_State *state = dfa ->get_start_state();
	int last_accepting = -1;
	while (simulateDone < 2) {
		cout << *lexeme_begin << "  " << *forward << "\n";
		char input_char = *forward;
		//longest matched token
		if (state ->is_accepting_state())
			last_accepting = state->get_token_id();

		state = state ->get_dfa_transition(input_char);

		//check dead state
		if (state == NULL) {
			lexeme = "";		//reset lexem and token name
			token_name = "";

			if (last_accepting == -1) //report exception
			{
				if(lexeme_begin == forward)
					fout << "Exception   => " <<  *forward<<"\n";
				else
					fout << "Exception   => " <<create_lexem(lexeme_begin, forward)<<"\n";
				//recover
				if(forward == lexeme_begin)
				{
					forward++;
					lexeme_begin++;
					forward_counter++;
				}
				else
				{
					lexeme_begin = forward;
				}
			} else {
				//if(lexeme_begin == forward)

				//else
					lexeme = create_lexem(lexeme_begin, forward);
				//get token name
				token_name = tokens->at(last_accepting);

				symbol_table[lexeme] = token_name;
				cout << "lexem = " << lexeme << "     " << "Token = "<< token_name << "\n";
				if(token_name.compare("White Space") != 0)
					fout<<token_name<<"\n";
				lexeme_begin = forward;
			}
			last_accepting = -1;
			state = dfa->get_start_state();
			//lexeme_begin = forward;
			lexeme_begin_buffer_num = forward_buffer_num;
			lexeme_begin_counter = forward_counter;
			update = false;
			//counter--;

		}
		if(update)
		{
			forward++; //update pointer
			forward_counter++;
		}
		else
			{
			update = true;
			}

		//handle buffers
		if (forward_counter == BUFFER_SIZE  && forward_buffer_num == 1) //at end end buffer one
		{
			buffer2 = read_file();
			forward = buffer2;
			forward_buffer_num = 2;
			forward_counter = 0;
			//cout<< "buf2"<<get_buffer_contents(buffer2);

		} else if (forward_counter == BUFFER_SIZE  && forward_buffer_num == 2) //at end end buffer two
		{
			buffer1 = read_file();
			forward = buffer1;
			forward_buffer_num = 1;
			forward_counter = 0;
			//cout<<"buf 1"<< get_buffer_contents(buffer1);

		}
		if(simulateDone>0)
			simulateDone++;
		if(*forward == '\0' )
			simulateDone++;

	}
	fout.close();
}

Driver::~Driver() {
	is.close();
	symbol_table.clear();
	//delete symbol_table;
}

int main3() {
	NFA *n;
	n = new NFA(2);
	DFA *d;
	d = new DFA(n);
	vector<string> *v = new vector<string> ();
	v->push_back("as1");
	v->push_back("as2");
	v->push_back("as3");
	v->push_back("as4");

	Driver *div;
	div = new Driver(d, "readme", v);

	//char *p = div ->read_file();
	//cout << div->get_buffer_contents(p);

	//char *p2 = div ->read_file();
	//cout << div->get_buffer_contents(p2);

	div->simulate();

	return 0;
}
