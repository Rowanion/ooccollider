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
	ss << mPriSystemTime->tm_year+1900 << "/" << mPriSystemTime->tm_mon+1 << "/" << mPriSystemTime->tm_mday << "-" << mPriSystemTime->tm_hour << ":" << mPriSystemTime->tm_min << "h";
    printf("Aktuelle Zeit: %s", asctime(mPriSystemTime));



//	if (fs::is_directory(_filename)){
//		if (fs::exists(_filename)){
//			_filename.replace_extension(ss.str()+_filename.extension());
//		}
//		else {
//
//		}
//	}
	mPriOfStream.open(_filename, ios::out);
	mPriOfStream << "mtllib " << ss.str() <<  endl;
}

Log::~Log() {
	mPriOfStream.flush();
	mPriOfStream.close();
}

std::ostream& Log::operator<< (int& val )
{
	mPriOfStream << val;
	return mPriOfStream;
}

}
