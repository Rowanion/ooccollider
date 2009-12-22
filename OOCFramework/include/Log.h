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

class Log {
	friend class Logger;
public:
	Log(boost::filesystem::path _filename);
	Log(const Log& _log);
	virtual ~Log();
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

//template <class T>
//Log* operator<<(T& _a, const Log* _log) //C++ function template sample
//{
//	_log->mPriOfStream << _a;
//	return _log;
//}

}

#endif /* LOG_H_ */
