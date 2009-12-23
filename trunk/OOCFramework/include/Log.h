/**
 * @file	Log.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 22.12.2009
 * @brief 	Log class declaration.
 */

#ifndef LOG_H_
#define LOG_H_

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "sys/time.h"
#include <iostream>

namespace oocframework {


//std::ostream& operator<<(std::ostream& _stream);
#define CSV ","
#define NL "\n"

class Log {
	friend class Logger;
public:
	Log(boost::filesystem::path _filename);
	Log(const Log& _log);
	virtual ~Log();
	void newTest(std::string  _title);
	std::ostream& operator<< (int& val );
	std::ostream& operator<< (float& val );
	std::ostream& operator<< (double& val );
	std::ostream& operator<< (std::string& val );

	boost::filesystem::ofstream mPriOfStream;
private:
	boost::filesystem::path mPriPath;
	time_t mPriTime;
	struct tm* mPriSystemTime;

	void setup(boost::filesystem::path _filename);

};

std::ostream& operator<<(Log& out, char c );
std::ostream& operator<<(Log& out, signed char c );
std::ostream& operator<<(Log& out, unsigned char c );

std::ostream& operator<<(Log& out, const char* s );
std::ostream& operator<<(Log& out, const signed char* s );
std::ostream& operator<<(Log& out, const unsigned char* s );

}

#endif /* LOG_H_ */
