#ifndef PARSER_H
#define PARSER_H

#include <memory>

#include "lexer.h"
#include "tree_node.h"

class Parser {
public:
	Parser() {
	}
	
	explicit Parser(std::istream& in) {
		setInput(in);
	}
	
	void setInput(std::istream& in) {
		m_lexer.setInput(in);
		nextTok();
	}
	
	Node* getG();
	Node* getP();
	Node* getF();
	Node* getT();
	Node* getE();
	Node* getArgs();
	Node* getCommaArgs();
	
private:
	
	void eatToken(TokenType tok, const char* msg) {
		if (m_next_tok->getType() != tok) {
			throw ParserException(msg, m_next_tok->getLine(), m_next_tok->getPos());
		}
		nextTok();
	}
	
	void nextTok() {
		m_next_tok.reset(m_lexer.getToken());
	}
	
	std::unique_ptr<Token> m_next_tok;
	Lexer m_lexer;
	
};


#endif
