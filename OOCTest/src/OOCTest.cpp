/**
 * @file	OOCTest.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.11.2009
 * @brief 	OOCTest class declaration.
 */

#include "MpiControl.h"
#include "CCollisionProtocol.h"
#include "OctreeHandler.h"
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <boost/lambda/lambda.hpp>
#include <algorithm>

#define PRESELECTED_SEED 670274678
#define BASE_MODEL_PATH "/home/ava/Diplom/Model/SampleTree"

using namespace std;
using namespace oocframework;
using namespace oocformats;
using namespace ooctools;

int main(int argc, char *argv[]) {
	oocframework::MpiControl* mpic = oocframework::MpiControl::getSingleton();

	// init nodes
	mpic->init(argc, argv);

	if (mpic->getRank() == 0) {




//		   set<int*, IntPtrLess > ssp;
//		   vector<int*> iVec = vector<int*>();
//		   iVec.resize(3);
//		   int* a = new int(13);
//		   int* b = new int(1);
//		   int* c = new int(4);
//		   iVec[0] = (a);
//		   iVec[1] = (b);
//		   iVec[2] = (c);
//		   for (unsigned i=0; i< iVec.size(); ++i){
//			   cerr << i << ": " << *iVec[i] << " (" << (uint64_t)iVec[i] << ")" << endl;
//		   }
//		   std::sort(iVec.begin(), iVec.end(), *boost::lambda::_1 < *boost::lambda::_2 );
//		   cerr << "------------------------------------------" << endl;
//		   for (unsigned i=0; i< iVec.size(); ++i){
//			   cerr << i << ": " << *iVec[i] << " (" << (uint64_t)iVec[i] << ")" << endl;
//		   }
//		   cerr << "------------------------------------------" << endl;
//		   vector<int> iVec2 = vector<int>();
//		   iVec2.resize(3);
//		   iVec2[0] = (*a);
//		   iVec2[1] = (*b);
//		   iVec2[2] = (*c);
//		   for (unsigned i=0; i< iVec2.size(); ++i){
//			   cerr << i << ": " << iVec2[i] << " (" << (uint64_t)&iVec2[i] << ")" << endl;
//		   }
//		   std::sort(iVec2.begin(), iVec2.end(), boost::lambda::_1 < boost::lambda::_2 );
//		   cerr << "------------------------------------------" << endl;
//		   for (unsigned i=0; i< iVec2.size(); ++i){
//			   cerr << i << ": " << iVec2[i] << " (" << (uint64_t)&iVec2[i] << ")" << endl;
//		   }
//	//	   std::sort( begin, end, *_1 > *_2 );
//		   // Now you can loop through the iterators and print strings in
//		      //alphabetical order.
//
//		exit(0);









		OctreeHandler oh = OctreeHandler();
		CCollisionProtocol ccp = CCollisionProtocol(PRESELECTED_SEED, 2);
		LooseOctree* lo = oh.loadLooseOctreeSkeleton(fs::path(string(BASE_MODEL_PATH)+"/skeleton.bin"));
		map<uint64_t, LooseOctree*> idLoMap = map<uint64_t, LooseOctree*>();
		oh.generateIdLoMap(lo, idLoMap);


		ccp.generateDistribution(lo);

		vector<ooctools::Quintuple> quintVec = vector<ooctools::Quintuple>();
		quintVec.push_back(Quintuple(1, 17.4, 1, 0, 0));
		quintVec.push_back(Quintuple(1, 17.4, 1, 1, 0));
		quintVec.push_back(Quintuple(1, 17.4, 1, 2, 0));
		quintVec.push_back(Quintuple(1, 17.4, 1, 3, 0));
		quintVec.push_back(Quintuple(1, 17.4, 1, 4, 0));
		quintVec.push_back(Quintuple(1, 17.4, 1, 5, 0));
		quintVec.push_back(Quintuple(1, 17.4, 1, 6, 0));
		quintVec.push_back(Quintuple(1, 17.4, 1, 7, 0));
		quintVec.push_back(Quintuple(1, 17.4, 1, 8, 0));
		quintVec.push_back(Quintuple(1, 17.4, 2, 3, 0));
		quintVec.push_back(Quintuple(1, 17.4, 3, 3, 0));
		quintVec.push_back(Quintuple(1, 17.4, 4, 3, 0));

		map<int, set<ooctools::Quintuple> > nodeReqMap = map<int, set<ooctools::Quintuple> >();

		ccp.doCCollision(&quintVec, &nodeReqMap);
		map<int, set<ooctools::Quintuple> >::iterator mapIt = nodeReqMap.begin();
		for (; mapIt != nodeReqMap.end(); mapIt++){
			cerr << "Distribution for Node " << mapIt->first << " (" << mapIt->second.size() << " VBOs): " << endl;
			set<ooctools::Quintuple>::iterator setIt = mapIt->second.begin();
			for (; setIt != mapIt->second.end(); setIt++){
				cerr << "VBO-Id: " << setIt->id << ", (" << idLoMap[setIt->id]->getTriangleCount() << ")" << endl;
			}
			cerr << endl;
		}

		mpic->barrier();
	}
	else {
		mpic->barrier();
	}




	cout << "leaving depending part: " << mpic->getRank() << endl;
	mpic->finalize();
	return 0;
}
