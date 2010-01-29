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

#include <sys/mman.h>
#include <cstdio>
#include <fcntl.h>

#include "StructDefs.h"
#include "MpiControl.h"
#include "CCollisionProtocol.h"
#include "LooseRenderOctree.h"
#include "OctreeHandler.h"

#define SEED1 670274678
#define SEED2 1
#define SEED3 1
#define SEED SEED1
#define DATA_NODE_COUNT 10
#define LVL_OF_REDUNDANCY 2

#ifdef HOME
#define SKEL_FILE "/home/ava/Diplom/Model/SampleTree/skeleton.bin"
#define REQUEST_FILE "/media/ClemensHDD/Arminius_tests/requestFile_4render_pc2.bin"
#endif

#ifdef OFFICE
#define SKEL_FILE "/home/ava/Diplom/Model/SampleTree/skeleton.bin"
//#define REQUEST_FILE "/home/ava/requestFile_1render_pc2.bin"
#define REQUEST_FILE "/home/ava/workspace5/OOCCollider/requestFile_4RendererMiniCluster.bin"

#endif

using namespace std;
using namespace ooctools;
using namespace oocframework;
using namespace oocformats;
namespace fs = boost::filesystem;

CCollisionProtocol* ccp;
unsigned* results;

char* mapFile(fs::path _path, unsigned _fileSize, int& _fHandle)
{
	char *map;  /* mmapped array of int's */

	_fHandle = open(_path.string().c_str(), O_RDONLY);
	if (_fHandle == -1) {
		perror("Error opening file for reading");
		exit(EXIT_FAILURE);
	}
	map = (char*)mmap(0, _fileSize, PROT_READ, MAP_SHARED, _fHandle, 0);
	if (map == MAP_FAILED) {
		close(_fHandle);
		perror("Error mmapping the file");
		exit(EXIT_FAILURE);
	}
	return map;
}

void umapFile(char* _map, unsigned _fileSize, int& _fHandle)
{
	if (munmap(_map, _fileSize) == -1) {
		perror("Error un-mmapping the file");
	}

	close(_fHandle);
}

int main(int argc, char* argv[])
{
	fs::path path = fs::path(REQUEST_FILE);
	unsigned fSize = fs::file_size(path);
	int fHandle = 0;
	char* map = 0;
	char* ptr = 0;

	// usage: /~: OOCSimulator LvlOfRedundancy DataNodeCount
//	oocframework::MpiControl* mpic = oocframework::MpiControl::getSingleton();
//	mpic->init(1, mpicInitArgs);

		// first argument=seed, 2nd argument=lvlOfRedundancy
		//	ccp = new CCollisionProtocol(1, atoi(argv[0]));
		ccp = new CCollisionProtocol(SEED, LVL_OF_REDUNDANCY, DATA_NODE_COUNT, 5, 4+DATA_NODE_COUNT);
		OctreeHandler oh = OctreeHandler();
		LooseRenderOctree* lro = oh.loadLooseRenderOctreeSkeleton(fs::path(SKEL_FILE));
		ccp->generateDistribution(lro);
//		ccp = new CCollisionProtocol(670274678, 2);
		cerr << "7" << endl;
//		ccp->setNodeIDs(5, 4+3);
		cerr << "8" << endl;
		//	ccp->setNodeIDs(5, 4+atoi(argv[1]));

		// +1 because the last entry is the total load;
		results = new unsigned[11];
//		results = new unsigned[atoi(argv[1])+1];

	map = mapFile(path, fSize, fHandle);
	ptr = map;
//		fs::ifstream inFile;
//		inFile.open(path, ios::binary);
		cerr << "path: " << path << ", size: " << fSize << endl;
		Quintuple* qArray;
		unsigned qSize = 0;
//		inFile.seekg(0, ios::beg);
		bool debug = true;
		unsigned roundCount = 0;
		for (; ptr <= (map+fSize); ptr += sizeof(unsigned)+(sizeof(Quintuple)*qSize)){
//		while (ptr <= (map+fSize)){
			bool verbose = false;
			if (roundCount == 3491) verbose = true;
//			cerr << "Reading in Round " << roundCount << " ..." << endl;
			qSize = ((unsigned*)ptr)[0];
//			inFile.read((char*)&qSize, sizeof(unsigned));
			if (verbose) cerr << "Size: " << qSize << endl;
			if (qSize > 0){
				qArray = new Quintuple[qSize];
				memcpy((char*)qArray, (ptr+sizeof(unsigned)), sizeof(Quintuple)*qSize);
				if (verbose){
					cerr << "Number of requests: " << qSize << endl;
					for (unsigned i=0; i< qSize; i++){
						cerr << i << " Quintuple: " << qArray[i].id << ", lvl: " << qArray[i].lvl  << ", destin: " << qArray[i].destId  << ", dist: " << qArray[i].dist  << endl;
					}
				}
				if (verbose) cerr << "number of quintuples in this request: " << qSize << endl;
				if (verbose) {
					cerr << "starting c-collision..." << endl;
				}
				ccp->simCCollision(qArray, qSize, results);
				if (verbose) cerr << "done c-collision." << endl;

//			cerr << "Load: " << results[10] << endl;
				debug = false;
				delete[] qArray;
			}

			roundCount++;
		}
		cerr << "RoundCount: " << roundCount << endl;
//		inFile.close();
		delete[] results;
		delete ccp;
		delete lro;
		umapFile(map, fSize, fHandle);

	return 0;
}
