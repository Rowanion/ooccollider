/**
 * @file	MpiControl.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 07.05.2009
 * @brief 	MpiControl class definition.
 */

#include "MpiControl.h"

#include <queue>
#include <iostream>
#include <cstdlib>


#include "Message.h"
#include "ColorBufferEvent.h"
#include "DepthBufferEvent.h"
#include "NodeRequestEvent.h"
#include "ModelViewMatrixEvent.h"

using namespace std;

MpiControl*
MpiControl::instance = 0;

MpiControl*
MpiControl::getSingleton()
{
	if (MpiControl::instance==0){
		MpiControl::instance = new MpiControl();
	}
	return MpiControl::instance;
}

MpiControl::MpiControl() : mRank(0), mSize(0), mPriInRequests(std::queue<Message*>()), mPriOutRequests(std::queue<Message*>())

{
	MPI::Init();
	mRank = MPI::COMM_WORLD.Get_rank();
	mSize = MPI::COMM_WORLD.Get_size();

	mOrigGroup = MPI::COMM_WORLD.Get_group();

}

MpiControl::~MpiControl()
{
	mRenderComm.Free();
	mDataComm.Free();
	mRenderGroup.Free();
	mDataGroup.Free();
	mPriDataNodes.clear();
	mPriRenderNodes.clear();
}

void MpiControl::init()
{
	if (mSize == 2) { // project start - one master & one renderer
		mPriRenderNodes.push_back(1);
		if (mRank == 1)
			mGroup = RENDERER;
	}
	if (mSize == 3) { // one master & one renderer & data
		mPriRenderNodes.push_back(1);
		mPriDataNodes.push_back(2);
		if (mRank == 1)
			mGroup = RENDERER;
		else if (mRank == 1)
			mGroup = DATA;
	}
	else if (mSize == 4){ // first split - one master & two renderer & data
		mPriRenderNodes.push_back(1);
		mPriRenderNodes.push_back(2);
		mPriDataNodes.push_back(3);
		if (mRank >= 1 && mRank <= 2)
			mGroup = RENDERER;
		else if (mRank >= 3)
			mGroup = DATA;
	}
	else if (mSize > 5){ // final - one master & four renderer & rest -> data
		mPriRenderNodes.push_back(1);
		mPriRenderNodes.push_back(2);
		mPriRenderNodes.push_back(3);
		mPriRenderNodes.push_back(4);
		for (int i=5; i< mSize; ++i){
			mPriDataNodes.push_back(i);
		}
		if (mRank >= 1 && mRank <= 4)
			mGroup = RENDERER;
		else if (mRank >= 5)
			mGroup = DATA;
	}
}

void MpiControl::receive(int src)
{
//	cout << mRank << " is receiving from " << src << "..." << endl;
	MPI::Status stat;
	while (!MPI::COMM_WORLD.Iprobe(src, MPI_ANY_TAG, stat))
	{	}
	int count = stat.Get_count(MPI_CHAR);
	int type = stat.Get_tag();
	int realSrc = stat.Get_source();
	Message* msg = new Message();
	msg->setType(type);
	msg->setLength(count);
	msg->setSrc(realSrc);
	msg->mData = new char[count];
	MPI::COMM_WORLD.Recv(msg->mData, count, MPI_CHAR, src, type, stat);
//	cout << "received " << msg->getType() << " from " << msg->getSrc() << endl;
	mInQueue.push(msg);
}

//TODO this way is not efficient
// it waits blocking for a msg from each group member. what happens if node 1 sends twice and 2 never sends?
void MpiControl::receive(Group _group)
{
//	cout << mRank << " is receiving from " << src << "..." << endl;
	MPI::Status stat;
	switch (_group){
	case RENDERER:{
		for(unsigned i=0; i< mPriRenderNodes.size(); ++i){
			receive(mPriRenderNodes[i]);
		}
		break;}
	case DATA:{
		for(unsigned i=0; i< mPriDataNodes.size(); ++i){
			receive(mPriDataNodes[i]);
		}
		break;}
	default:
		break;
	}
}

Message* MpiControl::directReceive(const oocframework::ClassId* classid)
{
		MPI::Status stat;
		while (!MPI::COMM_WORLD.Iprobe(MPI_ANY_SOURCE, classid->getShortId(), stat))
		{		}
		int count = stat.Get_count(MPI_CHAR);
		int type = stat.Get_tag();
		int realSrc = stat.Get_source();
		Message* msg = new Message();
		msg->setType(type);
		msg->setLength(count);
		msg->setSrc(realSrc);
		msg->mData = new char[count];
		MPI::COMM_WORLD.Recv(msg->mData, count, MPI_CHAR, realSrc, type, stat);
	//	cout << "received " << msg->getType() << " from " << msg->getSrc() << endl;
		return msg;
}

//Message* MpiControl::directReceive(int src)
//{
////	cout << mRank << " is receiving from " << src << "..." << endl;
//	MPI::Status stat;
//	while (!MPI::COMM_WORLD.Iprobe(src, MPI_ANY_TAG, stat)){
//	}
//	int count = stat.Get_count(MPI_CHAR);
//	int type = stat.Get_tag();
//	char* data = new char[count];
//	int realSrc = stat.Get_source();
//	MPI::COMM_WORLD.Recv(data, count, MPI_CHAR,src, type, stat);
//	Message* msg = new Message(type, count, mRank, data);
//	msg->setSrc(realSrc);
////	cout << "received " << msg->getType() << " from " << msg->getSrc() << endl;
//	delete[] data;
//	return msg;
//}

bool MpiControl::ireceive(int src)
{
	MPI::Status stat;
	MPI::Request req;
//	req.Wait(stat);

	Message* msg = 0;
	unsigned queueSize = mPriInRequests.size();

	if (MPI::COMM_WORLD.Iprobe(src, MPI_ANY_TAG, stat)){
		msg = new Message();
		int count = stat.Get_count(MPI_CHAR);
		int type = stat.Get_tag();
		msg->mData = new char[count];
		int realSrc = stat.Get_source();
//		cout << mRank << " started ireceiving from " << realSrc << endl;
		req = MPI::COMM_WORLD.Irecv(msg->mData, count, MPI_CHAR, src, type);
		msg->setType(type);
		msg->setLength(count);
		msg->setDst(mRank);
		msg->setSrc(realSrc);
		msg->request = req;
		mPriInRequests.push(msg);
	}


	for (unsigned i=0; i<queueSize; ++i){
		msg = mPriInRequests.front();
		mPriInRequests.pop();
		if (msg->request.Test()){
//			cout << "received " << msg->getType() << " from " << msg->getSrc() << endl;
			mInQueue.push(msg);
		}
		else{
			mPriInRequests.push(msg);
		}
	}

	return !mInQueue.empty();
}

bool MpiControl::ireceive(Group _group)
{
//	cout << mRank << " is receiving from " << src << "..." << endl;
	MPI::Status stat;
	bool result = false;
	switch (_group){
	case RENDERER:{
		for(unsigned i=0; i< mPriRenderNodes.size(); ++i){
			result |= ireceive(mPriRenderNodes[i]);
		}
		break;}
	case DATA:{
		for(unsigned i=0; i< mPriDataNodes.size(); ++i){
			result |= ireceive(mPriDataNodes[i]);
		}
		break;}
	default:
		break;
	}
	return result;
}

void MpiControl::completeWaitingReceives(const oocframework::ClassId* classid)
{
	MPI::Status stat;
	unsigned queueSize = mPriInRequests.size();
	Message* msg = 0;
	for (unsigned i=0; i<queueSize; ++i){
		msg = mPriInRequests.front();
		mPriInRequests.pop();
		msg->request.Get_status(stat);
		if (stat.Get_tag() == classid->getShortId()){
			msg->request.Wait();
			mInQueue.push(msg);
		}
		else{
			mPriInRequests.push(msg);
		}
	}
}

void MpiControl::send(Message* msg)
{
//	cout << mRank << " is sending to " << msg->getDst() << "..." << endl;
	MPI::Status stat;
	if (msg != 0){
		switch (msg->getGroup()){
		case RENDERER:{
			for(unsigned i=0; i< mPriRenderNodes.size(); ++i){
				MPI::COMM_WORLD.Ssend(msg->getData(), msg->getLength(), MPI_CHAR, mPriRenderNodes[i], msg->getType());
			}
			delete msg;
			break;}
		case DATA:{
			for(unsigned i=0; i< mPriDataNodes.size(); ++i){
				MPI::COMM_WORLD.Ssend(msg->getData(), msg->getLength(), MPI_CHAR, mPriDataNodes[i], msg->getType());
			}
			delete msg;
			break;}
		case ALL:{
			for(int i=0; i< mSize; ++i){
				if (mRank != i){
					MPI::COMM_WORLD.Ssend(msg->getData(), msg->getLength(), MPI_CHAR, i, msg->getType());
				}
			}
			delete msg;
			break;}
		default:
			MPI::COMM_WORLD.Ssend(msg->getData(), msg->getLength(), MPI_CHAR, msg->getDst(), msg->getType());
			delete msg;
			break;
		}
//		cout << "sending immediate msg...." << endl;
//		cout << mRank << " to " << msg->getDst() << "" << endl;
//		cout << "deleted msg!" << endl;
	}
	else if (!mOutQueue.empty()){
		Message* msg = mOutQueue.front();
		MPI::COMM_WORLD.Ssend(msg->getData(), msg->getLength(), MPI_CHAR,msg->getDst(), msg->getType());
		mOutQueue.pop();
//		cout << "send " << msg->getType() << " to " << msg->getDst() << endl;
		delete msg;
	}
}

void MpiControl::isend(Message* msg)
{
	Message* tempMsg = 0;

	if (msg != 0){
		cout << mRank << " is sending to " << msg->getDst() << "..." << endl;
		cout << "group: " << msg->getGroup() << endl;
		switch (msg->getGroup()){
		case RENDERER:{
			for(unsigned i=0; i< mPriRenderNodes.size(); ++i){
				tempMsg = new Message(*msg);
				tempMsg->request = MPI::COMM_WORLD.Isend(msg->getData(), msg->getLength(), MPI_CHAR, mPriRenderNodes[i], msg->getType());
				mPriOutRequests.push(tempMsg);
			}
			delete msg;
			msg = 0;
			break;}
		case DATA:{
			for(unsigned i=0; i< mPriDataNodes.size(); ++i){
				tempMsg = new Message(*msg);
				tempMsg->request = MPI::COMM_WORLD.Isend(msg->getData(), msg->getLength(), MPI_CHAR, mPriDataNodes[i], msg->getType());
				mPriOutRequests.push(tempMsg);
			}
			delete msg;
			msg = 0;
			break;}
		case ALL:{
		cout << "sending to all!" << endl;
			for(int i=0; i< mSize; ++i){
				if (mRank != i){
				cout << mRank << ": sending to all.... " << (msg->getType()==ModelViewMatrixEvent::classid()->getShortId())<< endl;
					tempMsg = new Message(*msg);
					tempMsg->request = MPI::COMM_WORLD.Isend(msg->getData(), msg->getLength(), MPI_CHAR, i, msg->getType());
					mPriOutRequests.push(tempMsg);
				}
			}
			delete msg;
			msg = 0;
			break;}
		default:
			msg->request = MPI::COMM_WORLD.Isend(msg->getData(), msg->getLength(), MPI_CHAR, msg->getDst(), msg->getType());
			mPriOutRequests.push(msg);
			break;
		}
//		msg = 0;
	}
	else if (!mOutQueue.empty()){
		msg = mOutQueue.front();
		mOutQueue.pop();
		msg->request = MPI::COMM_WORLD.Isend(msg->getData(), msg->getLength(), MPI_CHAR, msg->getDst(), msg->getType());
		mPriOutRequests.push(msg);
//		msg = 0;
	}

	unsigned queueSize = mPriOutRequests.size();
	for (unsigned i=0; i<queueSize; ++i){
		msg = mPriOutRequests.front();
		mPriOutRequests.pop();
		if (msg->request.Test()){
//			cout << "received " << msg->getType() << " from " << msg->getSrc() << endl;
			delete msg;
		}
		else{
			mPriOutRequests.push(msg);
		}
	}

}

void MpiControl::sendAll()
{
//	cout << "sending all from " << mRank << "..." << endl;
	Message* msg = 0;
	MPI::Status stat;
	if (!mOutQueue.empty()){
		for (int i=0; i< mSize; ++i){
			if (i!=mRank){
				msg = mOutQueue.front();
				MPI::COMM_WORLD.Ssend(msg->getData(), msg->getLength(), MPI_CHAR, i, msg->getType());
			}
		}
		mOutQueue.pop();
//		cout << "send " << msg->getType() << " to " << dst << endl;
		delete msg;
	}
}


Message* MpiControl::pop()
{
	if (!mInQueue.empty()){
		Message* msg = mInQueue.front();
		mInQueue.pop();
		return msg;
	}
	else return 0;
}

void MpiControl::push(Message* msg)
{
	//TODO ensure that all groupings work as they should
	switch (msg->getGroup()){
	case DEFAULT:
		mOutQueue.push(msg);
		break;
	case ALL:{
		for (int i=0; i< mSize; ++i){
			if (i != mRank){
				msg->setDst(i);
				mOutQueue.push(new Message(*msg));
			}
		}
		delete msg;
		break;}
	case RENDERER:{
		for (unsigned i=0; i< mPriRenderNodes.size(); ++i){
			if (mPriRenderNodes[i] != mRank){
				msg->setDst(mPriRenderNodes[i]);
				mOutQueue.push(new Message(*msg));
			}
		}
		delete msg;
		break;}
	case DATA:{
		for (unsigned i=0; i< mPriDataNodes.size(); ++i){
			if (mPriDataNodes[i] != mRank){
				msg->setDst(mPriDataNodes[i]);
				mOutQueue.push(new Message(*msg));
			}
		}
		delete msg;
		break;}
	}
}

void MpiControl::finalize()
{
	MPI::Finalize();
}

void MpiControl::makeRenderGroup(int size, const int* ranks)
{
	mRenderGroup = mOrigGroup.Incl(size,ranks);
	mRenderComm = MPI::COMM_WORLD.Create(mRenderGroup);
}

void
MpiControl::makeDataGroup(int size, const int* ranks)
{
	mDataGroup = mOrigGroup.Incl(size,ranks);
	mDataComm = MPI::COMM_WORLD.Create(mDataGroup);
}

void
MpiControl::clearOutQueue(int dst)
{
	Message* msg = 0;
	for (unsigned i = 0; i < mOutQueue.size(); ++i){
		msg = mOutQueue.front();
		mOutQueue.pop();
		if (dst >= 0){
			if (msg->getDst() == dst){
				delete msg;
				msg = 0;
			}
			else {
				mOutQueue.push(msg);
				msg = 0;
			}
		}
		else {
			delete msg;
			msg = 0;
		}
	}
}

void
MpiControl::clearOutQueue(Group _group)
{
	switch (_group){
	case ALL:
		clearOutQueue(-1);
		break;
	case RENDERER:
		for (unsigned i=0; i< mPriRenderNodes.size(); ++i){
			clearOutQueue(mPriRenderNodes[i]);
		}
		break;
	case DATA:
		for (unsigned i=0; i< mPriDataNodes.size(); ++i){
			clearOutQueue(mPriDataNodes[i]);
		}
		break;
	default:
		return;
		break;
	}
}

void
MpiControl::clearInQueue(int src)
{
	Message* msg = 0;
	for (unsigned i = 0; i < mInQueue.size(); ++i){
		msg = mInQueue.front();
		mInQueue.pop();
		if (src >= 0){
			if (msg->getSrc() == src){
				delete msg;
				msg = 0;
			}
			else {
				mInQueue.push(msg);
				msg = 0;
			}
		}
		else {
			delete msg;
			msg = 0;
		}
	}
}

void
MpiControl::clearInQueue(Group _group)
{
	switch (_group){
	case ALL:
		clearInQueue(-1);
		break;
	case RENDERER:
		for (unsigned i=0; i< mPriRenderNodes.size(); ++i){
			clearInQueue(mPriRenderNodes[i]);
		}
		break;
	case DATA:
		for (unsigned i=0; i< mPriDataNodes.size(); ++i){
			clearInQueue(mPriDataNodes[i]);
		}
		break;
	default:
		return;
		break;
	}
}

unsigned
MpiControl::getGroupSize(Group _group) const
{
	switch (_group){
	case ALL:
		return mSize;
		break;
	case RENDERER:
		return mPriRenderNodes.size();
		break;
	case DATA:
		return mPriDataNodes.size();
		break;
	default:
		return 0;
		break;
	}
}

const std::vector<int>& MpiControl::getRenderGroup() const
{
	return mPriRenderNodes;
}

const std::vector<int>& MpiControl::getDataGroup() const
{
	return mPriDataNodes;
}

void
MpiControl::debug()
{
	MPI::Group origGroup, renderGroup, dataGroup;
	MPI::Intracomm renderComm, dataComm;
	/* Extract the original group handle */
//	MPI_Group_incl(orig_group, NPROCS/2, ranks1, &new_group);
//	new_comm.Send(0, 1, MPI::INT, 0, 0);
	origGroup = MPI::COMM_WORLD.Get_group();
	int ranks = 1;
	renderGroup.Incl(1,&ranks);
	renderComm.Create(renderGroup);
}

//void
//MpiControl::sendImmediate(Message* msg)
//{
//	MPI::COMM_WORLD.Ssend(msg->getData(), msg->getLength(), MPI_CHAR, msg->getDst(), msg->getType());
//}
//
//void
//MpiControl::sendImmediateToAll(Message* msg)
//{
//	//TODO
//	for(int i=0; i< mSize; ++i){
//		if (i!=mRank){
//			MPI::COMM_WORLD.Ssend(msg->getData(), msg->getLength(), MPI_CHAR, i, msg->getType());
//		}
//	}
//}

