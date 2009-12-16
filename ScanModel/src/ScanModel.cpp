/**
 * @file	ScanModel.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 15.12.2009
 * @brief 	ScanModel class declaration.
 */

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "StructDefs.h"

#define BASE_MODEL_PATH "/media/ClemensHDD/SampleTree_packed"
#define BYTE_LIMIT 500000000

using namespace std;
using namespace ooctools;

struct Header{
	uint64_t id;
	unsigned indexCount;
	unsigned vertexCount;
	unsigned sizeOf();
};

unsigned Header::sizeOf(){
	return (sizeof(uint64_t) + 2*sizeof(unsigned) + indexCount*sizeof(unsigned) + vertexCount*sizeof(V4N4));
}

void writeToFile(fs::path _path)
{
	stringstream ss;

	if (fileCount < 0){
		fileCount = 0;
		ss << fileCount;
		outFile.open(fs::path(string(BASE_MODEL_PATH)+"/Data"+ss.str()+".bin"), ios::binary);
		ss.flush();
	}
	else if (byteCount > BYTE_LIMIT){
		outFile.flush();
		outFile.close();
		fileCount++;
		ss << fileCount;
		outFile.open(fs::path(string(BASE_DEST_PATH)+"/Data"+ss.str()+".bin"), ios::binary);
		ss.flush();
		byteCount = 0;
		// close file and open new
	}

	Header head;
	head.id = strToUint64(_path.stem());


	idxFile.open(_path, ios::binary);
	dataFile.open(_path.parent_path() / (_path.stem()+".data"), ios::binary);

	idxFile.seekg(0, ios::beg);
	idxFile.read((char*)&head.indexCount, sizeof(unsigned));
	idxFile.seekg(4, ios::beg);
	dataFile.seekg(0, ios::beg);
	dataFile.read((char*)&head.vertexCount, sizeof(unsigned));
	dataFile.seekg(4, ios::beg);

	indexData = new unsigned[head.indexCount];
	vertexData = new ooctools::V4N4[head.vertexCount];

	idxFile.read((char*)indexData, sizeof(unsigned)*head.indexCount);
	idxFile.close();

	dataFile.read((char*)vertexData, sizeof(V4N4)*head.vertexCount);
	dataFile.close();

	// ------------------------------------
	//write to file

	outFile.seekp(byteCount, ios::beg);
	//writing header-info
	outFile.write((char*)&head, sizeof(Header));
	outFile.seekp(byteCount+sizeof(Header), ios::beg);
	//writing the indices
	outFile.write((char*)indexData, head.indexCount*sizeof(unsigned));
	outFile.seekp(byteCount + sizeof(Header) + head.indexCount*sizeof(unsigned), ios::beg);
	//writing the vertices
	outFile.write((char*)vertexData, head.vertexCount*sizeof(V4N4));
	byteCount += head.sizeOf();
	outFile.flush();

	delete[] vertexData;
	vertexData = 0;
	delete[] indexData;
	indexData = 0;
}

void recurseDir(fs::path _path)
{
	fs::directory_iterator end_itr; // default construction yields past-the-end
	for (fs::directory_iterator itr(_path); itr != end_itr; ++itr) {
		if (fs::is_directory(itr->status())) {
			recurseDir(itr->path());
		}
		else if (itr->path().extension() == ".idx") {
			writeToFile(itr->path());
		}
	}

}

int main(int argc, char* argv[])
{
	return 0;
}
