/**
 * @file	OOCSimulator.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 28.01.2010
 */

#include "mpi.h"

#include "boost/filesystem.hpp"
#include <boost/filesystem/fstream.hpp>

#include <iostream>

#include "StructDefs.h"
#include "MpiControl.h"
#include "CCollisionProtocol.h"
#include "LooseRenderOctree.h"
#include "OctreeHandler.h"

using namespace std;
using namespace ooctools;
using namespace oocframework;
using namespace oocformats;
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
	char* mpicInitArgs[2];
	mpicInitArgs[0] = "1";
	mpicInitArgs[1] = argv[2];
//	oocframework::MpiControl* mpic = oocframework::MpiControl::getSingleton();
//	mpic->init(1, mpicInitArgs);

		// first argument=seed, 2nd argument=lvlOfRedundancy
		//	ccp = new CCollisionProtocol(1, atoi(argv[0]));
		ccp = new CCollisionProtocol(670274678, 2, 10, 5, 14);
		OctreeHandler oh = OctreeHandler();
		LooseRenderOctree* lro = oh.loadLooseRenderOctreeSkeleton(fs::path("/home/ava/Diplom/Model/SampleTree/skeleton.bin"));
		ccp->generateDistribution(lro);
//		ccp = new CCollisionProtocol(670274678, 2);
		cerr << "7" << endl;
//		ccp->setNodeIDs(5, 4+3);
		cerr << "8" << endl;
		//	ccp->setNodeIDs(5, 4+atoi(argv[1]));

		// +1 because the last entry is the total load;
		results = new unsigned[11];
//		results = new unsigned[atoi(argv[1])+1];


		fs::path path = fs::path("/media/ClemensHDD/Arminius_tests/requestFile_4render_pc2.bin");
		fs::ifstream inFile;
		inFile.open(path, ios::binary);
		unsigned fSize = fs::file_size(path);
		cerr << "path: " << path << ", size: " << fSize << endl;
		Quintuple* qArray;
		unsigned qSize = 0;
		inFile.seekg(0, ios::beg);
		bool debug = true;
		while (!inFile.eof()){
			inFile.read((char*)&qSize, sizeof(unsigned));
			qArray = new Quintuple[qSize];
			inFile.read((char*)qArray, sizeof(Quintuple)*qSize);
			ccp->simCCollision(qArray, qSize, results);

			if (debug){
				cerr << "Number of requests: " << qSize << endl;
			for (unsigned i=0; i< qSize; i++){
				cerr << "Quintuple: " << qArray[i].id << ", lvl: " << qArray[i].lvl  << ", destin: " << qArray[i].destId  << ", dist: " << qArray[i].dist  << endl;
			}
			}
			cerr << "Load: " << results[10] << endl;
			delete[] qArray;
			debug = false;
		}

		inFile.close();
		delete[] results;
		delete ccp;
		delete lro;

	return 0;
}
