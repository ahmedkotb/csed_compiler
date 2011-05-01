/*
 * Combined_NFA.cpp
 *
 *  Created on: May 1, 2011
 *      Author: ahmed
 */

#include "Parsing_output.h"

Parsing_output::Parsing_output(NFA* combinedNFA , vector<string>* tokens) {
	this->combined = combinedNFA;
	this->lan_tokens = tokens;
}

Parsing_output::~Parsing_output() {
	// TODO Auto-generated destructor stub
}

NFA* Parsing_output::get_combinedNFA()
{
	return combined;
}

vector<string>* Parsing_output::get_lan_tokens(){
	return this->lan_tokens;
}
