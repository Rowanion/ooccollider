/**
 * @file	Logger.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 22.12.2009
 * @brief 	Logger class declaration.
 */

#include "Logger.h"

using namespace std;

namespace fs=boost::filesystem;

namespace oocframework{

Logger* Logger::mPriInstancePtr = 0;

Logger* Logger::getSingleton()
{
	if (mPriInstancePtr == 0)
	{
		mPriInstancePtr = new Logger();
	}
	return mPriInstancePtr;
}

Logger::Logger() {
	// TODO Auto-generated constructor stub

}

Logger::~Logger() {
	closeAllHandles();
}

void Logger::closeAllHandles()
{
	mPriLogList.clear();
}

Log* Logger::newLog(boost::filesystem::path _filename)
{
	Log l = Log(_filename);
	mPriLogList.push_back(l);
	return &mPriLogList.back();
}

} //oocframework
