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

CCollisionProtocol* ccp;
unsigned* results;

void manageCCollision()
{

}

void proceedFrame(Quintuple* _qArr, unsigned _arrSize)
{
	// distribution of requests from here
}

int main(int argc, char* argv[])
{
	// usage: /~: OOCSimulator LvlOfRedundancy DataNodeCount


	// first argument=seed, 2nd argument=lvlOfRedundancy
//	ccp = new CCollisionProtocol(1, atoi(argv[0]));
	ccp = new CCollisionProtocol(1, 1);
	ccp->setNodeIDs(5, 4+8);
//	ccp->setNodeIDs(5, 4+atoi(argv[1]));

	// +1 because the last entry is the total load;
	results = new unsigned[atoi(argv[1])+1];


	fs::path path = fs::path("/home/ava/workspace5/OOCCollider/requestFile_4render_pc2.bin");
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
		inFile.read((char*)qArray, sizeof(Quintuple)*qSize);
		ccp->simCCollision(qArray, qSize, results);

//		for (unsigned i=0; i< qSize; i++){
//			cerr << "Quintuple: " << qArray[i].id << ", lvl: " << qArray[i].lvl  << ", destin: " << qArray[i].destId  << ", dist: " << qArray[i].dist  << endl;
//		}
		delete[] qArray;
	}

	inFile.close();
	delete[] results;
	delete ccp;
	cerr << "path: " << path << endl;
	return 0;
}
