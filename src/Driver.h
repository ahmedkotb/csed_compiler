/*
 * Driver.h
 *
 *  Created on: May 3, 2011
 *      Author: Ahmed Mohsen
 */

#ifndef DRIVER_H_
#define DRIVER_H_
#define BUFFER_SIZE 4096
#define OUTPUT_FILE "./tests/out.txt"
#include "DFA.h"
#include <iostream>
#include <fstream>


class Driver {

private:
	//symbol table to hold each lexme with corresponding token
	map<string , string> symbol_table;

	long total_input_size;

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

	//constructor given attributes
	string src_file_name;
	DFA *dfa;
	vector<string> *tokens;

	string create_lexem(char *start , char *end);



public:
	char* read_file();

	Driver(DFA *my_dfa , string file_name , vector<string>*token_names);
	map<string , string> get_symbol_table();
	string get_buffer_contents(char *buffer);
	void simulate();
	virtual ~Driver();
};

#endif /* DRIVER_H_ */
