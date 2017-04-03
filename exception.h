#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "utils.h"

#include <exception>

class Exception : public std::exception {
public:
	
	Exception(const char* message) noexcept :
		std::exception(),
		m_message(message)
	{
	}
	
	virtual 
	const char* what() const noexcept {
		return m_message;
	}
	
	virtual ~Exception() {}
	
protected:
	
	const char* m_message;
};

class LexerException : public Exception {
public:
	LexerException(const char* message, int line, int pos) noexcept :
		Exception(message), 
		m_line(line),
		m_pos(pos)
	{
	}
	
	int getLine() const {
		return m_line;
	}
	
	int getPos() const {
		return m_pos;
	}
	
protected:
	
	int m_line;
	int m_pos;
};

class IOException : public Exception {
public:
	IOException(const char* message) noexcept :
		Exception(message)
	{
	}
};

class ParserException : public Exception {
public:
	ParserException(const char* message, int line, int pos) noexcept :
		Exception(message), 
		m_line(line),
		m_pos(pos)
	{
	}

	int getLine() const {
		return m_line;
	}
	
	int getPos() const {
		return m_pos;
	}
	
protected:
	
	int m_line;
	int m_pos;
};
#endif
