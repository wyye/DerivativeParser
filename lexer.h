#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <istream>
#include <cctype>

#include "debug.h"
#include "exception.h"

enum class TokenType {
	ADD,
	SUB,
	DIV,
	MUL,
	POW,
	ID,
	OPEN_PAR,
	CLOSE_PAR,
	OPEN_BRACK,
	CLOSE_BRACK,
	DIGIT,
	COMMA,
	END
};

struct Token {
	Token(TokenType tt, int line, int pos) :
		m_type(tt),
		m_line(line),
		m_pos(pos)
	{}
	TokenType getType() const {
		return m_type;
	}
	
	virtual 
	~Token() {}
	
	
	virtual 
	double getDigit() const {
		ASSERT(0);
		return 0;
	}
	
	virtual
	std::string getId() const {
		ASSERT(0);
		return "";
	}
	
	int getLine() const {
		return m_line;
	}
	
	int getPos() const {
		return m_pos;
	}
private:
	TokenType m_type;
	int m_line;
	int m_pos;
};

struct IdToken : public Token {
	template <typename T>
	IdToken(T&& value, int line, int pos) :
		Token(TokenType::ID, line, pos),
		m_value(std::forward<T>(value))
	{
	}
	
	virtual 
	std::string getId() const override {
		return m_value;
	}
private:
	std::string m_value;
};

struct DigitToken : public Token {
	DigitToken(double value, int line, int pos) :
		Token(TokenType::DIGIT, line, pos),
		m_value(value)
	{
	}
	
	virtual 
	double getDigit() const override {
		return m_value;
	}
	
private:
	double m_value;
};

class Lexer {
public:
	Lexer() {}
	
	explicit Lexer(std::istream& expr) {
		setInput(expr);
	}
	
	void setInput(std::istream& expr)
	{
		m_expr = &expr;
		m_line = 0;
		m_pos = 0;
		m_next = 0;
		m_end = false;
		getSym();
	}
	
	Token* getToken() {
		ASSERT(m_expr != nullptr);
		
		getSpaces();
		
		if (m_end) {
			return new Token(TokenType::END, m_line, m_pos);
		} else if (m_next == '+') {
			nextSym();
			return new Token(TokenType::ADD, m_line, m_pos);
		} else if (m_next == '-') {
			nextSym();
			return new Token(TokenType::SUB, m_line, m_pos);
		} else if (m_next == '*') {
			nextSym();
			return new Token(TokenType::MUL, m_line, m_pos);
		} else if (m_next == '/') {
			nextSym();
			return new Token(TokenType::DIV, m_line, m_pos);
		} else if (m_next == '^') {
			nextSym();
			return new Token(TokenType::POW, m_line, m_pos);
		} else if (m_next == '(') {
			nextSym();
			return new Token(TokenType::OPEN_PAR, m_line, m_pos);
		} else if (m_next == ')') {
			nextSym();
			return new Token(TokenType::CLOSE_PAR, m_line, m_pos);
		} else if (m_next == '[') {
			nextSym();
			return new Token(TokenType::OPEN_BRACK, m_line, m_pos);
		} else if (m_next == ']') {
			nextSym();
			return new Token(TokenType::CLOSE_BRACK, m_line, m_pos);
		} else if (m_next == ',') {
			nextSym();
			return new Token(TokenType::COMMA, m_line, m_pos);
		} else if (std::isalpha(m_next)) {
			return new IdToken(getId(), m_line, m_pos);
		} else if (isdigit(m_next)) {
			return new DigitToken(getDigit(), m_line, m_pos);
		} else {
			throw LexerException("Unknown token", m_line, m_pos);
		}
	}
	
private:
	void getSym() {
		if (!m_expr->get(m_next)) {
			if (m_expr->eof()) m_end = 1;
			else throw IOException("Can't read input stream");
		}
	}
	
	void nextPos() {
		if (m_next == '\n'){
			m_line += 1;
			m_pos = 0;
		} else {
			m_pos++;
		}
	}
	
	void nextSym() {
		if (m_end) return;
		
		nextPos();
		getSym();
	}
	
	void getSpaces() {
		while (std::isspace(m_next))
			getSpace();
	}
	
	void getSpace() {
		if (std::isspace(m_next)) nextSym();
	}
	
	std::string getId() {
		std::string id;
		while (isalnum(m_next) && !m_end) {
			id += m_next;
			nextSym();
		}
		return id;
	}
	
	double getDigit() {
		std::string num;
		
		ASSERT(isdigit(m_next));
		num += m_next;
		nextSym();
		
		while (isdigit(m_next) && !m_end) {
			num += m_next;
			nextSym();
		}
		
		if (m_next == '.' && !m_end) {
			num+=m_next;
			nextSym();
			
			if (!(isdigit(m_next) && !m_end)) 
				throw LexerException("Digit expected", m_line, m_pos);
			
			while (isdigit(m_next) && !m_end) {
				num += m_next;
				nextSym();
			}
		}
		
		return std::stod(num);
	}
	
	int m_line;
	int m_pos;
	char m_next;
	bool m_end;
	std::istream* m_expr = nullptr;
};

#endif
