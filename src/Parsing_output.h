/*
 * Combined_NFA.h
 *
 *  Created on: May 1, 2011
 *      Author: ahmed
 */

#ifndef COMBINED_NFA_H_
#define COMBINED_NFA_H_
#include "NFA.h"
#include <string>
#include <vector>

class Parsing_output {
public:
	Parsing_output(NFA* combinedNFA , vector<string>* tokens);
	virtual ~Parsing_output();
	NFA* get_combinedNFA();
	vector<string>* get_lan_tokens();
private :
	NFA* combined;
	vector<string>* lan_tokens;
};

#endif /* COMBINED_NFA_H_ */
