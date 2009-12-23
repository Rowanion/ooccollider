/**
 * @file	Log.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 22.12.2009
 * @brief 	Log class declaration.
 */

#include "Log.h"
#include <sstream>

namespace fs=boost::filesystem;
using namespace std;

namespace oocframework {

Log::Log(boost::filesystem::path _filename) {
	mPriPath = _filename;
	setup(mPriPath);
}

Log::Log(const Log& _log)
{
	mPriPath = _log.mPriPath;
	setup(mPriPath);
}

void Log::setup(boost::filesystem::path _filename)
{

	time(&mPriTime);
    mPriSystemTime = localtime(&mPriTime);
	stringstream ss;
//	ss << mPriSystemTime->tm_year+1900 << "-" << mPriSystemTime->tm_mon+1 << "-" << mPriSystemTime->tm_mday << "_" << mPriSystemTime->tm_hour << "." << mPriSystemTime->tm_min << "h";
	ss << mPriSystemTime->tm_year+1900 << "-" << mPriSystemTime->tm_mon+1 << "-" << mPriSystemTime->tm_mday;
    printf("Aktuelle Zeit: %s", asctime(mPriSystemTime));
    _filename.replace_extension(ss.str()+_filename.extension());

    //append to existing file
    if (fs::exists(_filename)){
    	mPriOfStream.open(_filename, ios::in|ios::out);
    	mPriOfStream.seekp(0, ios::end);
    }
    else {
    	mPriOfStream.open(_filename, ios::out);
    }
}

Log::~Log() {
	mPriOfStream.flush();
	mPriOfStream.close();
}

void Log::newTest(std::string  _title)
{
	mPriOfStream << endl << _title << CSV;
}

std::ostream& Log::operator<< (int& val )
{
	mPriOfStream << val << CSV;
	return mPriOfStream;
}

std::ostream& Log::operator<< (float& val )
{
	mPriOfStream << val << CSV;
	return mPriOfStream;
}

std::ostream& Log::operator<< (double& val )
{
	mPriOfStream << val << CSV;
	return mPriOfStream;
}

std::ostream& Log::operator<< (std::string& val )
{
	mPriOfStream << val << CSV;
	return mPriOfStream;
}

std::ostream& operator<< (Log& out, char c )
{
	out.mPriOfStream << c << CSV;
	return out.mPriOfStream;
}
std::ostream& operator<< (Log& out, signed char c )
{
	out.mPriOfStream << c << CSV;
	return out.mPriOfStream;
}
std::ostream& operator<< (Log& out, unsigned char c )
{
	out.mPriOfStream << c << CSV;
	return out.mPriOfStream;
}

std::ostream& operator<< (Log& out, const char* s )
{
	out.mPriOfStream << s << CSV;
	return out.mPriOfStream;
}

std::ostream& operator<< (Log& out, const signed char* s )
{
	out.mPriOfStream << s << CSV;
	return out.mPriOfStream;
}

std::ostream& operator<< (Log& out, const unsigned char* s )
{
	out.mPriOfStream << s << CSV;
	return out.mPriOfStream;
}

}
