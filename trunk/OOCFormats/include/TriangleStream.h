/**
 * @file	TriangleStream.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.06.2009
 * @brief 	TriangleStream class declaration.
 */

#ifndef TRIANGLESTREAM_H_
#define TRIANGLESTREAM_H_

#include <string>

#include "boost/filesystem.hpp"
#include <boost/filesystem/fstream.hpp>

#include "Triangle.h"
#include "FileHeader.h"
#include "FileIO.h"
#include "declarations.h"

namespace oocformats {

/**
 * @class TriangleStream
 * @brief StreamReader/-Writer for Triangles.
 */
class TriangleStream {
public:
	enum Direction{
		IN,
		OUT
	};
	enum Method{
		APPEND,
		OVERWRITE
	};
	enum Position{
		BEGIN,
		END,
		CURRENT
	};
	TriangleStream();
	virtual ~TriangleStream();
	bool open(std::string fname, Direction dir, Method meth = OVERWRITE);
	bool open(fs::path file, Direction dir, Method meth = OVERWRITE);
	void close();
	bool isOpen() const {return mOpen;};
	bool read(ooctools::Triangle& tri);
	void write(const ooctools::Triangle& tri);
	unsigned getTriangleSize() const {return mTriSize;};
	int getVertexByteSize() const {return mTriSize*3*4*sizeof(float);};
	unsigned getNormalByteSize() const {return mTriSize*3*4*sizeof(char);};
	unsigned getTrianglePosition() const {return mTriPosition;};
	void rewind();
	bool checkHeaderVsFile();

private:
	bool mOpen;
	boost::filesystem::fstream mINormalStream;
	boost::filesystem::fstream mIVertexStream;
	boost::filesystem::fstream mONormalStream;
	boost::filesystem::fstream mOVertexStream;
	fs::path mPriVertexFile;
	fs::path mPriNormalFile;

	Direction mPriDir;
	Method mPriMeth;

	// number of triangles in filestream
	// in read-mode = number triangles in file
	// in write-mode = number of tris written in this file-stream so far
	unsigned mTriSize;
	unsigned mTriPosition;
	FileHeader mFh;
	FileIO mFio;
};

}

#endif /* TRIANGLESTREAM_H_ */
