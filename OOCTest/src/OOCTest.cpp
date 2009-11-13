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
		OctreeHandler oh = OctreeHandler();
		CCollisionProtocol ccp = CCollisionProtocol(PRESELECTED_SEED);
		LooseOctree* lo = oh.loadLooseOctreeSkeleton(fs::path(string(BASE_MODEL_PATH)+"/skeleton.bin"));

		ccp.generateDistribution(lo);

		vector<ooctools::Quintuple> quintVec = vector<ooctools::Quintuple>();
		quintVec.push_back(Quintuple(1, 17.4, 1, 4, 0));
//		quintVec.push_back(Quintuple(1, 17.4, 1, 1001, 0));
//		quintVec.push_back(Quintuple(1, 17.4, 1, 1002, 0));

		map<int, set<ooctools::Quintuple> > nodeReqMap = map<int, set<ooctools::Quintuple> >();

		ccp.doCCollision(&quintVec, &nodeReqMap);
		map<int, set<ooctools::Quintuple> >::iterator mapIt = nodeReqMap.begin();
		cout << "Distribution 4 Node " << mapIt->first << ": " << endl;
		for (; mapIt != nodeReqMap.end(); mapIt++){
			set<ooctools::Quintuple>::iterator setIt = mapIt->second.begin();
			for (; setIt != mapIt->second.end(); setIt++){
				cout << "VBO-Id: " << setIt->id << endl;
			}
		}

		mpic->barrier();
	}
	else {
		mpic->barrier();
	}


//	cout << "entering depending part" << endl;
//	if (mpic->getRank() == 0) {
//		cout << "MY RANK IS " << mpic->getRank() << endl;
//		RenderMasterCore rmc = RenderMasterCore(TARGET_WIDTH, TARGET_HEIGHT);
////		setupWindow("master");
//		cout << "End of display loop " << mpic->getRank()  << endl;
//		//		setupWindow("My rank is 0");
//	}
//	else if (mpic->getGroup() == oocframework::MpiControl::RENDERER){ // RENDER-GROUP
//		//		setupWindow("slave");
//		RenderCore rc = RenderCore(WINDOW_WIDTH, WINDOW_HEIGHT, TARGET_WIDTH, TARGET_HEIGHT);
//		cout << "End of display loop " << mpic->getRank() << endl;
//	}
//	else if (mpic->getGroup() == oocframework::MpiControl::DATA){ // DATA-GROUP
//		//		setupWindow("slave");
//		DataCore dc = DataCore(WINDOW_WIDTH, WINDOW_HEIGHT, TARGET_WIDTH, TARGET_HEIGHT);
//		cout << "End of display loop " << mpic->getRank() << endl;
//	}

	cout << "leaving depending part: " << mpic->getRank() << endl;
	mpic->finalize();
	return 0;
}
