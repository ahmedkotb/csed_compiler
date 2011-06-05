/*
 * Parse_Table_Entry.cpp
 *
 *  Created on: Jun 5, 2011
 *      Author: ahmedkotb
 */

#include "Parse_Table_Entry.h"

Parse_Table_Entry::Parse_Table_Entry() {
	sync = false;
	epsilon = false;
	production_num = -1;
}

bool Parse_Table_Entry::has_entry(){
	return (production_num != -1);
}


bool Parse_Table_Entry::is_sync(){
	return sync;
}

bool Parse_Table_Entry::is_epsilon(){
	return epsilon;
}

int Parse_Table_Entry::get_production_num(){
	return production_num;
}

void Parse_Table_Entry::set_production_num(int num){
	this->production_num = num;
	if (production_num != -1){
		epsilon = false;
		sync = false;
	}
}

void Parse_Table_Entry::set_sync(bool sync){
	this->sync = sync;
	if (sync)
		production_num = -1;
}

void Parse_Table_Entry::set_epsilon(bool epsilon){
	this->epsilon = epsilon;
	if (epsilon)
			production_num = -1;
}


Parse_Table_Entry::~Parse_Table_Entry() {
	// TODO Auto-generated destructor stub
}
