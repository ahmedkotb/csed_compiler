/*
 * Token.cpp
 *
 *  Created on: May 27, 2011
 *      Author: ahmedkotb
 */

#include "Token.h"

Token::Token(string lexeme , string type) {
	this->lexeme = lexeme;
	this->type = type;
}

string Token::get_token_lexeme(){
	return lexeme;
}

string Token::get_token_type(){
	return type;
}

void Token::set_token_lexeme(string lexeme){
	this->lexeme = lexeme;
}
void Token::set_token_type(string type){
	this->type = type;
}
Token::~Token() {

}
