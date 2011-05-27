/*
 * Token.h
 *
 *  Created on: May 27, 2011
 *      Author: ahmedkotb
 */

#ifndef TOKEN_H_
#define TOKEN_H_
#include <string>
using namespace std;
class Token {
private:
	string lexeme;
	string type;
public:
	Token(string,string);

	//getters
	string get_token_lexeme();
	string get_token_type();

	//setters
	void set_token_lexeme(string);
	void set_token_type(string);

	virtual ~Token();
};

#endif /* TOKEN_H_ */
