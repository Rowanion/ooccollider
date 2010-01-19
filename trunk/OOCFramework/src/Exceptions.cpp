/**
 * @file	Exceptions.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 19.01.2010
 * @brief 	Exceptions class declaration.
 */

#include "Exceptions.h"

#include <string>
#include <sstream>

namespace oocframework {

const char* TestException::what() const throw()
{
	return "My exception happened";
}

// -----------------------------------------------------------------------------
NullPointerException::NullPointerException(int _pos) : mPriPosition(_pos)
{}

const char* NullPointerException::what() const throw()
{
	std::stringstream ss;
	ss << "Null-Pointer Exception at position " << mPriPosition << "!" << std::endl;
	return ss.str().c_str();
}

// -----------------------------------------------------------------------------
OutOfRangeException::OutOfRangeException(int _pos) : mPriPosition(_pos)
{}

const char* OutOfRangeException::what() const throw()
{
	std::stringstream ss;
	ss << "Point OutOfRange Exception at position " << mPriPosition << "!" << std::endl;
	return ss.str().c_str();
}
// ------------------------------------------------------------------------------
OutOfMemoryException::OutOfMemoryException(int _pos, unsigned _free, unsigned _needed)
{}

const char* OutOfMemoryException::what() const throw()
{
	std::stringstream ss;
	ss << "Point OutOfMemory Exception at position " << mPriPosition << "!" << std::endl <<
	"Bytes free: " << mPriFree << "" << std::endl << "Bytes needed: " << mPriNeeded << "" << std::endl;
	return ss.str().c_str();
}

}
