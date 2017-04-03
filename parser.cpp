#include <iostream>
#include "parser.h"

#include <istream>
#include <cctype>
#include <memory>

Node* Parser::getG() {
	Node* e = getE();
	if (m_next_tok->getType() == TokenType::END) {
		return e;
	} else {
		throw ParserException(
			"Extra symbols after the end of expression",
			m_next_tok->getLine(), m_next_tok->getPos());
	}
}

Node* Parser::getE() {
	Node* tl = getT();
	Node* tr = nullptr;
	while (
		m_next_tok->getType() == TokenType::ADD || 
		m_next_tok->getType() == TokenType::SUB
	) {
		if (m_next_tok->getType() == TokenType::ADD) {
			nextTok();
			tr = getT();
			tl = new AddNode(tl, tr);
			tr = nullptr;
		} else if (m_next_tok->getType() == TokenType::SUB) {
			nextTok();
			tr = getT();
			tl = new SubNode(tl, tr);
			tr = nullptr;
		}
	}
	return tl;
}

Node* Parser::getT() {
	Node* fl = getF();
	Node* fr = nullptr;
	
	while (
		m_next_tok->getType() == TokenType::MUL || 
		m_next_tok->getType() == TokenType::DIV
	) {
		if (m_next_tok->getType() == TokenType::MUL) {
			nextTok();
			fr = getF();
			fl = new MulNode(fl, fr);
			fr = nullptr;
		} else if (m_next_tok->getType() == TokenType::DIV) {
			nextTok();
			fr = getF();
			fl = new DivNode(fl, fr);
			fr = nullptr;
		}
	}
	return fl;
}

Node* Parser::getF() {
	Node* ptop = getP();
	Node* pr = nullptr;
	BinaryNode* pr_parent = nullptr;
		
	while (m_next_tok->getType() == TokenType::POW) {
		nextTok();
		pr = getP();
		if (pr_parent == nullptr) {
			pr_parent = new PowNode(ptop, pr);
			ptop = pr_parent;
		} else {
			PowNode* new_right = new PowNode(pr_parent->getRight(), pr);
			pr_parent->setRight(new_right);
			pr_parent = new_right;
		}
	}
	
	return ptop;
}

Node* Parser::getP() {
	if (m_next_tok->getType() == TokenType::END) {
		throw ParserException(
			"Unexpected end of input",
			m_next_tok->getLine(), m_next_tok->getPos());
	} else if (m_next_tok->getType() == TokenType::ADD) {
		nextTok();
		Node* p = getP();
		return p;
	} else if (m_next_tok->getType() == TokenType::SUB) {
		nextTok();
		Node* p = getP();
		return new MinusNode(p);
	} else if (m_next_tok->getType() == TokenType::OPEN_PAR) {
		nextTok();
		Node* n = getE();
		eatToken(TokenType::CLOSE_PAR, "Expected closing parenthesis");
		return n;
	} else if (m_next_tok->getType() == TokenType::OPEN_BRACK) {
		nextTok();
		Node* e = getE();
		eatToken(TokenType::COMMA, "Comma expected");
		if (!(m_next_tok->getType() == TokenType::ID)) {
			throw ParserException(
				"Identifier expected",
				m_next_tok->getLine(), m_next_tok->getPos());
		}
		IdNode* id = new IdNode(m_next_tok->getId());
		nextTok();
		eatToken(TokenType::CLOSE_BRACK, "Expected closing bracket");
		return new DerivativeNode(e, id);
	} else if (m_next_tok->getType() == TokenType::DIGIT) {
		Node* n = new NumNode(m_next_tok->getDigit());
		nextTok();
		return n;
	} else if (m_next_tok->getType() == TokenType::ID) {
		IdNode* n = new IdNode(m_next_tok->getId());
		Node* ret_node = n;
		nextTok();
		if (m_next_tok->getType() == TokenType::OPEN_PAR) {
			nextTok();
			Node* args = getArgs();
			ArgsNode* cast_args = dynamic_cast<ArgsNode*>(args);
			ASSERT(cast_args != nullptr);
			ret_node = new FuncNode(n, cast_args);
			eatToken(TokenType::CLOSE_PAR, "Expected closing parenthesis");
		}
		return ret_node;
	} else {
		throw ParserException(
			"Unknown symbol in input stream", 
			m_next_tok->getLine(), m_next_tok->getPos());
	}

}

Node* Parser::getArgs() {
	if (m_next_tok->getType() == TokenType::END) {
		throw ParserException(
			"Unexpected end of input",
			m_next_tok->getLine(), m_next_tok->getPos());
	} else {
		if (m_next_tok->getType() == TokenType::CLOSE_PAR) {
			return new ArgsNode();
		} else {
			Node* n = getCommaArgs();
			return n;
		}
	}
}

Node* Parser::getCommaArgs() {
	if (m_next_tok->getType() == TokenType::END) {
		throw ParserException(
			"Unexpected end of input",
			m_next_tok->getLine(), m_next_tok->getPos());
	} else {		
		ArgsNode* args = new ArgsNode();
		args->push_back(getE());
		while (m_next_tok->getType() == TokenType::COMMA) {
			nextTok();
			args->push_back(getE());
		}
		return args;
	}
}
