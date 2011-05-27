/*
 * Lexical_Analyser.h
 *
 *  Created on: May 27, 2011
 *      Author: ahmedkotb
 */

#ifndef LEXICAL_ANALYSER_H_
#define LEXICAL_ANALYSER_H_

#include "Token.h"
#include "Parser.h"
#include "Parsing_output.h"
#include "NFA.h"
#include "DFA.h"
#include <iostream>
#include <fstream>

#define BUFFER_SIZE 8192


class Lexical_Analyser {

private:
	vector<string> * token_names;
	DFA* dfa;

	//2 buffers for reading the source files
	char *buffer1;
	char *buffer2;


	//2 pointers for buffers handling
	char *lexeme_begin;
	char *forward;

	//where the above pointer points to ... 1 or 2
	int lexeme_begin_buffer_num;
	int forward_buffer_num;

	//pointers counters
	int lexeme_begin_counter;
	int forward_counter;

	//input stream
	ifstream is;


	string get_lexeme(char *start , char *end);

	void fill_buffer(char * buffer);

public:

	Lexical_Analyser(string input_file_path , string source_file_path);

	Token * get_next_token();

	virtual ~Lexical_Analyser();
};

#endif /* LEXICAL_ANALYSER_H_ */
