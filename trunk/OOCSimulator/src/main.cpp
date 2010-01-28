/**
 * @file	OOCSimulator.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 28.01.2010
 */

#include "boost/filesystem.hpp"
#include <boost/filesystem/fstream.hpp>

#include <iostream>

#include "StructDefs.h"
#include "MpiControl.h"
#include "CCollisionProtocol.h"

using namespace std;
using namespace ooctools;
using namespace oocframework;
//using namespace oocformats;
namespace fs = boost::filesystem;

int main(int argc, char* argv[])
{
	fs::path path = fs::path("/home/ava/workspace5/OOCCollider/requestFile.bin");
	fs::ifstream inFile;
	inFile.open(path, ios::binary);
	unsigned fSize = fs::file_size(path);
	cerr << "path: " << path << ", size: " << fSize << endl;
	Quintuple* qArray;
	unsigned qSize = 0;
	inFile.seekg(0, ios::beg);
	while (!inFile.eof()){
		inFile.read((char*)&qSize, sizeof(unsigned));
		qArray = new Quintuple[qSize];
		cerr << "Number of requests: " << qSize << endl;
		unsigned pos = inFile.tellg();
		inFile.read((char*)qArray, sizeof(Quintuple)*qSize);
		for (unsigned i=0; i< qSize; i++){
			cerr << "Quintuple: " << qArray[i].id << ", lvl: " << qArray[i].lvl  << ", destin: " << qArray[i].destId  << ", dist: " << qArray[i].dist  << endl;
		}
		delete[] qArray;
	}

	inFile.close();
	cerr << "path: " << path << endl;
	return 0;
}
