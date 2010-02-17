/**
 * @file	OOCSimulator.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 28.01.2010
 */

#include <algorithm>
#include <boost/lambda/lambda.hpp>


#include "boost/filesystem.hpp"
#include <boost/filesystem/fstream.hpp>

#include <iostream>
#include <sstream>
#include <vector>

#include <sys/mman.h>
#include <cstdio>
#include <fcntl.h>


#include "StructDefs.h"
#include "MpiControl.h"
#include "CCollisionProtocol.h"
#include "LooseRenderOctree.h"
#include "OctreeHandler.h"
#include "Logger.h"
#include "Log.h"

#define SEED1 670274678
#define SEED2 2447873201
#define SEED3 382658925
#define SEED SEED1
#define DATA_NODE_COUNT 120
#define LVL_OF_REDUNDANCY 1

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
vector<unsigned> results;
Log* l;

struct Wrap{
	unsigned i;
};

bool operator<(const Wrap& _lhs, const Wrap& _rhs)
{
	return (_lhs.i<_rhs.i);
}

void evalLog(vector<unsigned>& _res, unsigned _size, Log* l)
{
	unsigned avg = _res[_size-1];
	unsigned size = _size-1;
	float median = 0;
	float minQuantil = 0.0;
	float maxQuantil = 0.0;
	unsigned total = 0;
	for (unsigned i=0; i< size; i++){
		total += _res[i];
	}
	float fract = 1.0f/total;
	vector<unsigned>::iterator it = _res.end();
	it--;
	sort(_res.begin(), it);
	for (unsigned i=1; i< size; i++){
		if (_res[i-1]>_res[i]){
			cerr << "notSorted!" << endl;
		}
//		cerr << _res[i-1] << ", ";
	}
//	cerr << _res[size-1] << endl;

	if (size%2==0){
		median = 0.5f*(_res[size/2]+_res[(size/2)-1]);
	}
	else {
		median = _res[(size-1)/2];
	}
	if (size > 0 && size <= 10){
		minQuantil = _res[1];
		maxQuantil = _res[size-2];
	}
	else if (size > 10 && size <= 20){
		minQuantil = _res[2];
		maxQuantil = _res[size-3];
	}
	else if (size > 20 && size <= 30){
		minQuantil = _res[3];
		maxQuantil = _res[size-4];
	}
	else if (size > 30 && size <= 40){
		minQuantil = _res[4];
		maxQuantil = _res[size-5];
	}
	else if (size > 40 && size <= 50){
		minQuantil = _res[5];
		maxQuantil = _res[size-6];
	}
	else if (size > 50 && size <= 60){
		minQuantil = _res[6];
		maxQuantil = _res[size-7];
	}
	else if (size > 60 && size <= 70){
		minQuantil = _res[7];
		maxQuantil = _res[size-8];
	}
	else if (size > 70 && size <= 80){
		minQuantil = _res[8];
		maxQuantil = _res[size-9];
	}
	else if (size > 80 && size <= 90){
		minQuantil = _res[9];
		maxQuantil = _res[size-10];
	}
	else if (size > 90 && size <= 100){
		minQuantil = _res[10];
		maxQuantil = _res[size-11];
	}
	else if (size > 100 && size <= 110){
		minQuantil = _res[11];
		maxQuantil = _res[size-12];
	}
	else if (size > 110 && size <= 120){
		minQuantil = _res[12];
		maxQuantil = _res[size-13];
	}
	else if (size > 120 && size <= 130){
		minQuantil = _res[13];
		maxQuantil = _res[size-14];
	}
	float avgQuantil = 0.5*(minQuantil+maxQuantil);
	float minNormQuantil = minQuantil*fract;
	float maxNormQuantil = maxQuantil*fract;
	float avgNormQuantil = avgQuantil*fract;
	float normMedian = median*fract;

	float relError = maxNormQuantil-avgNormQuantil;
	(*l) << avgNormQuantil;
	(*l) << relError;
	(*l) << normMedian;
	(*l) << avg;
}

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
	stringstream ss;
	ss << "cCollision_Seed1_Redun" << LVL_OF_REDUNDANCY << "_DataNodes" << DATA_NODE_COUNT << ".log";

	l = &(Logger::getSingleton()->newLog(fs::path(ss.str())));
	l->addCommentLine("center_of_error_range,relError,median,average");

	fs::path path = fs::path(REQUEST_FILE);
	unsigned fSize = fs::file_size(path);
	int fHandle = 0;
	char* map = 0;
	char* ptr = 0;

	ccp = new CCollisionProtocol(SEED, LVL_OF_REDUNDANCY, DATA_NODE_COUNT, 5, 4+DATA_NODE_COUNT);
	OctreeHandler oh = OctreeHandler();
	LooseRenderOctree* lro = oh.loadLooseRenderOctreeSkeleton(fs::path(SKEL_FILE));
	ccp->generateDistribution(lro);

	// +1 because the last entry is the total load;
	results.resize(DATA_NODE_COUNT+1);
//	results = new unsigned[DATA_NODE_COUNT+1];

	map = mapFile(path, fSize, fHandle);
	ptr = map;
	cerr << "path: " << path << ", size: " << fSize << endl;
	Quintuple* qArray;
	unsigned qSize = 0;
	bool debug = true;
	unsigned roundCount = 0;
	for (; ptr <= (map+fSize); ptr += sizeof(unsigned)+(sizeof(Quintuple)*qSize)){
		bool verbose = false;
		if (roundCount == 3491) verbose = true;
		qSize = ((unsigned*)ptr)[0];
		if (verbose) cerr << "Size: " << qSize << endl;
		if (qSize > 0){
			qArray = new Quintuple[qSize];
			memcpy((char*)qArray, (ptr+sizeof(unsigned)), sizeof(Quintuple)*qSize);
			if (roundCount == 2259){
				cerr << endl;
				verbose = true;
			}
			if (verbose){
				cerr << "Number of requests: " << qSize << endl;
				for (unsigned i=0; i< qSize; i++){
					cerr << i << " Quintuple: " << qArray[i].id << ", lvl: " << qArray[i].lvl  << ", destin: " << qArray[i].destId  << ", dist: " << qArray[i].dist  << ", tris: " << lro->getTriangleCount() << endl;
				}
			}
			if (verbose) cerr << "number of quintuples in this request: " << qSize << endl;
			if (verbose) {
				cerr << "starting c-collision..." << endl;
			}
			ccp->simCCollision(qArray, qSize, results);
			l->newTest();
//			if (roundCount%2 == 0){
				(*l) << roundCount;
				(*l) << qSize;
				evalLog(results, DATA_NODE_COUNT+1, l);
//			}
			if (verbose) cerr << "done c-collision." << endl;
//			if (verbose) exit(0);

			//			cerr << "Load: " << results[10] << endl;
			debug = false;
			delete[] qArray;
		}

		roundCount++;
	}
	cerr << "RoundCount: " << roundCount << endl;
//	delete[] results;
	delete ccp;
	delete lro;
	umapFile(map, fSize, fHandle);
	Logger::getSingleton()->closeAllHandles();

	return 0;
}
