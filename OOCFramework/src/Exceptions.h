/**
 * @file	Exceptions.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 19.01.2010
 * @brief 	Exceptions class declaration.
 */

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <exception>

namespace oocframework {

class TestException : public std::exception{
public:
	virtual const char* what() const throw();
};

class NullPointerException : public std::exception{
public:
	NullPointerException(int _pos);
	virtual const char* what() const throw();
private:
	int mPriPosition;
};

class OutOfRangeException : public std::exception{
public:
	OutOfRangeException(int _pos);
	virtual const char* what() const throw();
private:
	int mPriPosition;
};

class OutOfMemoryException : public std::exception{
public:
	OutOfMemoryException(int _pos, unsigned _free, unsigned _needed);
	virtual const char* what() const throw();
private:
	int mPriPosition;
	unsigned mPriFree;
	unsigned mPriNeeded;
};

} // oocframewok

#endif /* EXCEPTIONS_H_ */
