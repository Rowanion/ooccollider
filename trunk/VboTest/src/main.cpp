/**
 * @file	main.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 15.12.2009
 * @brief 	main class declaration.
 */

#include "VboFactory.h"
#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include "LooseRenderOctree.h"


namespace fs=boost::filesystem;
using namespace oocframework;
using namespace ooctools;
using namespace oocframework;
using namespace std;

int main(int argc, char* argv[]){
	VboFactory* vf = VboFactory::getSingleton();
	list<oocformats::WrappedOcNode*> myList = list<oocformats::WrappedOcNode*>();
	list<oocformats::WrappedOcNode*>::iterator a,b,c,d,e,f,g;
	oocformats::WrappedOcNode* won;
	fs::ifstream inFile;
	IVbo* v1, *v2, *v3, *v4;
	inFile.open("/home/ava/Diplom/Model/SampleTree_packed/Data0.bin", ios::binary);
	IVbo* iVbo = vf->newVbo(&inFile, 0);
	cerr << "ivbo: " << iVbo->getByteSize()<< endl;
	int count = 1;
	while(iVbo != 0){
		won = new oocformats::WrappedOcNode();
		won->timeStamp = count;
		won->iVbo = iVbo;
		myList.push_back(won);
		if (count == 27){
			v1 = iVbo;
		}
		else if (count == 127){
			v2 = iVbo;
		}
		else if (count == 128){
			v3 = iVbo;
		}
		else if (count == 543){
			v4 = iVbo;
		}
		count++;
//		cerr << "count: " << count << endl;
//		cerr << "add: " << (uint64_t)vf->findFirstFit(200)->address << endl;
////		cerr << "add: " << (uint64_t)((char*)iVbo)+iVbo->getByteSize() << endl;
//		cerr << "test: " << (1024*1024*64) - (837*80124) << endl;
//		cerr << "size: " << iVbo->getByteSize() << endl;
//		cerr << "vertexData: " << (int)iVbo->vertexData()[2342].n[3]<< endl;
//		cerr << "id: " << iVbo->getId()<< endl;
//		cerr << "idxCount: " << iVbo->getIndexCount()<< endl;
//		cerr << "vertexCount: " << iVbo->getVertexCount()<< endl;
		iVbo = vf->newVbo(&inFile, 0);
	}
	vf->debug();
	unsigned i =0;
	for (list<oocformats::WrappedOcNode*>::iterator wIt = myList.begin(); wIt != myList.end(); ++wIt, ++i){
		if(i == 27){
			a = wIt;
			cerr << "a: " << (uint64_t)(*a)->iVbo << endl;
		}
		else if (i == 127){
			b = wIt;
			cerr << "b: " << (uint64_t)(*b)->iVbo << endl;
		}
		else if (i == 128){
			c = wIt;
			cerr << "c: " << (uint64_t)(*c)->iVbo << endl;
		}
		else if (i == 543){
			d = wIt;
			cerr << "d: " << (uint64_t)(*d)->iVbo << endl;
		}
	}
	g = myList.begin();
	e = g++;
	f = g++;
	cerr << "e: " << (uint64_t)(*e)->iVbo << endl;
	cerr << "f: " << (uint64_t)(*f)->iVbo << endl;
	vf->debug();
	cerr << "deleting the vbos...." << endl;
	vf->freeVbo((*g)->iVbo);
	(*g)->iVbo = 0;
	cerr << "a: " << (uint64_t)(*a)->iVbo << " - " << (*a)->iVbo->mPriByteSize << endl;
	cerr << "b: " << (uint64_t)(*b)->iVbo << " - " << (*b)->iVbo->mPriByteSize << endl;
	cerr << "c: " << (uint64_t)(*c)->iVbo << " - " << (*c)->iVbo->mPriByteSize << endl;
	cerr << "d: " << (uint64_t)(*d)->iVbo << " - " << (*d)->iVbo->mPriByteSize << endl;

	vf->debug();
	vf->freeVbo((*f)->iVbo);
	(*f)->iVbo = 0;
	vf->debug();
	vf->freeVbo((*e)->iVbo);
	(*e)->iVbo = 0;
//	vf->freeVbo((*b)->iVbo);
//	(*b)->iVbo = 0;
//	for (list<oocformats::WrappedOcNode*>::iterator wIt = myList.begin(); wIt != myList.end(); ++wIt){
//		if ((*wIt)->iVbo == v1 || (*wIt)->iVbo == v2 || (*wIt)->iVbo == v3 || (*wIt)->iVbo == v4){
//			vf->freeVbo((*wIt)->iVbo);
//			(*wIt)->iVbo = 0;
//		}
//	}
	vf->debug();

	vf->defrag(&myList);
	vf->debug();
	return 0;
}
