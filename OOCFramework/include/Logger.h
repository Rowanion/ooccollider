/**
 * @file	Logger.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 22.12.2009
 * @brief 	Logger class declaration.
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <list>

#include "Log.h"

namespace oocframework{

class Logger {
public:
	virtual ~Logger();
	static Logger* getSingleton();
	Log& newLog(boost::filesystem::path _filename);
	void closeAllHandles();
private:
	Logger();
	static Logger* mPriInstancePtr;

	std::list<Log> mPriLogList;
};

} //oocframework
#endif /* LOGGER_H_ */
