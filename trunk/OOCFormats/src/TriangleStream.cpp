/**
 * @file	TriangleStream.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.06.2009
 * @brief 	TriangleStream class definition.
 */

#include "TriangleStream.h"

#include <iostream>

#include "FileIO.h"
#include "V3ub.h"
#include "BoundingBox.h"

using namespace ooctools;
using namespace std;
namespace fs = boost::filesystem;
namespace oocformats {

TriangleStream::TriangleStream() : mOpen(false), mTriSize(0), mTriPosition(0), mFio(FileIO())
{
	// TODO Auto-generated constructor stub

}

TriangleStream::~TriangleStream()
{
	if (mIVertexStream.is_open())
		mIVertexStream.close();
	if (mOVertexStream.is_open())
		mOVertexStream.close();
	if (mINormalStream.is_open())
		mINormalStream.close();
	if (mONormalStream.is_open())
		mONormalStream.close();
	// TODO Auto-generated destructor stub
}

bool
TriangleStream::open(std::string fStem, Direction dir, Method meth)
{
	if(mOpen)
		return false;

	mPriMeth = meth;
	mPriDir = dir;
	fs::path nFile = fs::path(fStem + ".na");
	fs::path vFile = fs::path(fStem + ".va");
	mPriVertexFile = vFile;
	mPriNormalFile = nFile;
	if (!fs::exists(nFile)){
		meth = OVERWRITE;
		mPriMeth = meth;
		mFh.bb = new BoundingBox();
		mFh.color = new V3ub((unsigned char)0);
	}
	else if (meth==APPEND||dir==IN) {
		mIVertexStream.open(vFile, ios::in|ios::binary);
		unsigned filesize = fs::file_size(mPriVertexFile);

		if (filesize==0){
			mFh.bb = new BoundingBox();
			mFh.color = new V3ub((unsigned char)0);
			mFh.nFaces = 0;
			mFh.nVertices = 0;
			mFh.nNormals = 0;
			mTriSize = 0;
		}
		else {
			mIVertexStream.seekg(0, ios::beg);
			//		mFh = FileIO::readHeader(mIVertexStream);
			//		mFh.bb = new BoundingBox(FileIO::readV3f(mIVertexStream),FileIO::readV3f(mIVertexStream));
			mFh.bb = mFio.readBB(mIVertexStream);
			mFh.nFaces = mFio.readUInt(mIVertexStream);
			mFh.nVertices = mFio.readInt(mIVertexStream);
			mFh.nNormals = mFio.readInt(mIVertexStream);
			mFh.color = mFio.readV3ub(mIVertexStream);
			mTriSize = mFh.nFaces;
		}
		mIVertexStream.close();
	}
	else {	// overwrite
		fs::remove(nFile);
		fs::remove(vFile);
		mFh.bb = new BoundingBox();
		mFh.color = new V3ub((unsigned char)0);
	}

	if (dir == OUT){
		mOpen = true;
		if (meth==APPEND){
			mOVertexStream.open(vFile,ios::in|ios::out|ios::binary);
			mONormalStream.open(nFile,ios::in|ios::out|ios::binary);
			mTriPosition = mFh.nFaces;
		}
		else{
			mOVertexStream.open(vFile,ios::out|ios::binary);
			mONormalStream.open(nFile,ios::out|ios::binary);
			mTriPosition = 0;
		}
		return true;
	}
	else{
		mIVertexStream.open(vFile,ios::in|ios::binary);
		mINormalStream.open(nFile,ios::in|ios::binary);
		mOpen = true;
		return true;
	}

	return false;
}

bool
TriangleStream::open(fs::path file, Direction dir, Method meth)
{
	if(mOpen)
		return false;

	mPriMeth = meth;
	mPriDir = dir;
	fs::path nFile;
	fs::path vFile;
	string ext = file.extension();
	if (ext[1]=='v'){
		nFile = fs::path(file.parent_path() / (file.stem() + ".na"));
		vFile = file;
	}
	else if (ext[1]=='n'){
		vFile = fs::path(file.parent_path() / (file.stem() + ".va"));
		nFile = file;
	}
	// assume old style
	else{
		nFile = fs::path(file.parent_path() / "nArray.bin");
		vFile = fs::path(file.parent_path() / "vArray.bin");
	}

	mPriVertexFile = vFile;
	mPriNormalFile = nFile;
	string test(nFile.string());
	if (!fs::exists(nFile)){
		meth = OVERWRITE;
		mPriMeth = meth;
		mFh.bb = new BoundingBox();
		mFh.color = new V3ub((unsigned char)0);
	}
	else if (meth==APPEND||dir==IN) {
		mIVertexStream.open(vFile, ios::in|ios::binary);
		unsigned filesize = fs::file_size(mPriVertexFile);
		if (filesize==0){
			mFh.bb = new BoundingBox();
			mFh.color = new V3ub((unsigned char)0);
			mFh.nFaces = 0;
			mFh.nVertices = 0;
			mFh.nNormals = 0;
			mTriSize = 0;
		}
		else {
			mIVertexStream.seekg(0, ios::beg);
			//		mFh = FileIO::readHeader(mIVertexStream);
			//		mFh.bb = new BoundingBox(FileIO::readV3f(mIVertexStream),FileIO::readV3f(mIVertexStream));
			mFh.bb = mFio.readBB(mIVertexStream);
			mFh.nFaces = mFio.readUInt(mIVertexStream);
			mFh.nVertices = mFio.readInt(mIVertexStream);
			mFh.nNormals = mFio.readInt(mIVertexStream);
			mFh.color = mFio.readV3ub(mIVertexStream);
			mTriSize = mFh.nFaces;
		}

		mIVertexStream.close();
	}
	else {	// overwrite
		fs::remove(nFile);
		fs::remove(vFile);
		mFh.bb = new BoundingBox();
		mFh.color = new V3ub((unsigned char)0);
	}

	if (dir == OUT){
		mOpen = true;
		if (meth==APPEND){
			mOVertexStream.open(vFile,ios::in|ios::out|ios::binary);
			mONormalStream.open(nFile,ios::in|ios::out|ios::binary);
			mTriPosition = mFh.nFaces;
		}
		else{
			mOVertexStream.open(vFile,ios::out|ios::binary);
			mONormalStream.open(nFile,ios::out|ios::binary);
			mTriPosition = 0;
		}
		return true;
	}
	else{
		mIVertexStream.open(vFile,ios::in|ios::binary);
		mINormalStream.open(nFile,ios::in|ios::binary);
		mOpen = true;
		mTriPosition = 0;
		return true;
	}

	return false;
}

void
TriangleStream::close()
{
	if (mOpen){
		if (mOVertexStream.is_open()){
			mOVertexStream.flush();
			mONormalStream.flush();
			unsigned filesize = 0;
			mOVertexStream.seekg(0, ios::end);
			filesize = fs::file_size(mPriVertexFile);

			if (filesize==0){
				mOVertexStream.close();
				mONormalStream.close();
				fs::remove(mPriVertexFile);
				fs::remove(mPriNormalFile);
			}
			else {
				mOVertexStream.close();
				mONormalStream.close();
			}
		}
		else if (mIVertexStream.is_open()){
			mIVertexStream.close();
			mINormalStream.close();
		}
		mOpen = false;
		delete mFh.bb;
		mFh.bb = 0;
		delete mFh.color;
		mFh.color = 0;
		mFh.nFaces = 0;
		mFh.nVertices = 0;
		mFh.nNormals = 0;
		mTriSize = 0;
		mTriPosition = 0;
	}
}

bool
TriangleStream::read(Triangle& tri)
{
	if (!mOpen || !mIVertexStream.is_open()){
		return false;
	}
	else {
		if (mTriPosition>=mTriSize){
			return false;
		}
		else{
			mIVertexStream.seekg(FileHeader::getHeaderSize()+
					(sizeof(float)*4*3*mTriPosition), ios::beg);
			mINormalStream.seekg(FileHeader::getHeaderSize()+
				(sizeof(char)*4*3*mTriPosition), ios::beg);
			mFio.readTriangleVertices(tri, mIVertexStream);
			mFio.readTriangleNormals(tri, mINormalStream);
			++mTriPosition;
			return true;
		}
	}
}

void
TriangleStream::write(const Triangle& tri)
{
	if (mOpen && mOVertexStream.is_open()){
		mFh.nVertices+=3;
		mFh.nNormals+=3;
		++mFh.nFaces;
		mFh.bb->expand(tri.getBb());
		mOVertexStream.seekp(0, ios::beg);
		mONormalStream.seekp(0, ios::beg);
		mFio.writeHeader(mFh, mOVertexStream);
		mFio.writeHeader(mFh, mONormalStream);
		mOVertexStream.flush();
		mONormalStream.flush();
		mOVertexStream.seekp(FileHeader::getHeaderSize()+
				(sizeof(float)*4*3*mTriPosition), ios::beg);
		mONormalStream.seekp(FileHeader::getHeaderSize()+
				(sizeof(char)*4*3*mTriPosition), ios::beg);
		mFio.writeTriangleVertices(tri, mOVertexStream);
		mFio.writeTriangleNormals(tri, mONormalStream);
		mOVertexStream.flush();
		mONormalStream.flush();
		++mTriSize;
		++mTriPosition;
	}
}

void
TriangleStream::rewind()
{
	if (mINormalStream.is_open()){
		mINormalStream.seekg(FileHeader::getHeaderSize(), ios::beg);
		mTriPosition = 0;
	}
	if (mIVertexStream.is_open()){
		mIVertexStream.seekg(FileHeader::getHeaderSize(), ios::beg);
		mTriPosition = 0;
	}
	if (mONormalStream.is_open()){
		mONormalStream.seekp(FileHeader::getHeaderSize(), ios::beg);
		mTriPosition = 0;
	}
	if (mOVertexStream.is_open()){
		mOVertexStream.seekp(FileHeader::getHeaderSize(), ios::beg);
		mTriPosition = 0;
	}
}

bool
TriangleStream::checkHeaderVsFile()
{
	if (mOVertexStream.is_open()){
		unsigned pos = mOVertexStream.tellp();
		mOVertexStream.seekp(0, ios::end);
		unsigned size = mOVertexStream.tellp();
		mOVertexStream.seekp(pos, ios::beg);
		unsigned fileTriCount = (size-FileHeader::getHeaderSize())/(3*4*4);
		if (fileTriCount != mTriSize){
			cerr << "size: " << size << endl;
			cerr << "unmatching file/header sizes: file(" << fileTriCount << "); header(" << mTriSize << ")" << endl;
			return false;
		}
	}
	if (mONormalStream.is_open()){
		unsigned pos = mONormalStream.tellp();
		mONormalStream.seekp(0, ios::end);
		unsigned size = mONormalStream.tellp();
		mONormalStream.seekp(pos, ios::beg);
		unsigned fileTriCount = (size-FileHeader::getHeaderSize())/(3*4);
		if (fileTriCount != mTriSize){
			cerr << "size: " << size << endl;
			cerr << "unmatching file/header sizes: file(" << fileTriCount << "); header(" << mTriSize << ")" << endl;
			return false;
		}
	}
	if (mIVertexStream.is_open()){
		unsigned pos = mIVertexStream.tellg();
		mIVertexStream.seekg(0, ios::end);
		unsigned size = mIVertexStream.tellg();
		mIVertexStream.seekg(pos, ios::beg);
		unsigned fileTriCount = (size-FileHeader::getHeaderSize())/(3*4*4);
		if (fileTriCount != mTriSize){
			cerr << "size: " << size << endl;
			cerr << "unmatching file/header sizes: file(" << fileTriCount << "); header(" << mTriSize << ")" << endl;
			return false;
		}
	}
	if (mINormalStream.is_open()){
		unsigned pos = mINormalStream.tellg();
		mINormalStream.seekg(0, ios::end);
		unsigned size = mINormalStream.tellg();
		mINormalStream.seekg(pos, ios::beg);
		unsigned fileTriCount = (size-FileHeader::getHeaderSize())/(3*4);
		if (fileTriCount != mTriSize){
			cerr << "size: " << size << endl;
			cerr << "unmatching file/header sizes: file(" << fileTriCount << "); header(" << mTriSize << ")" << endl;
			return false;
		}
	}
	return true;
}

} // oocformats
