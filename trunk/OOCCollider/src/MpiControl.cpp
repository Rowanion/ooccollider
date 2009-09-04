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
}

void MpiControl::receive(int src)
{
//	cout << mRank << " is receiving from " << src << "..." << endl;
	MPI::Status stat;
	while (!MPI::COMM_WORLD.Iprobe(src, MPI_ANY_TAG, stat))
	{	}
	int count = stat.Get_count(MPI_CHAR);
	int type = stat.Get_tag();
	char* data = new char[count];
	int realSrc = stat.Get_source();
	MPI::COMM_WORLD.Recv(data, count, MPI_CHAR,src, type, stat);
	Message* msg = new Message(type, count, mRank, data);
	msg->setSrc(realSrc);
//	cout << "received " << msg->getType() << " from " << msg->getSrc() << endl;
	mInQueue.push(msg);
	delete[] data;
}

Message* MpiControl::directReceive(int src)
{
//	cout << mRank << " is receiving from " << src << "..." << endl;
	MPI::Status stat;
	while (!MPI::COMM_WORLD.Iprobe(src, MPI_ANY_TAG, stat)){
	}
	int count = stat.Get_count(MPI_CHAR);
	int type = stat.Get_tag();
	char* data = new char[count];
	int realSrc = stat.Get_source();
	MPI::COMM_WORLD.Recv(data, count, MPI_CHAR,src, type, stat);
	Message* msg = new Message(type, count, mRank, data);
	msg->setSrc(realSrc);
//	cout << "received " << msg->getType() << " from " << msg->getSrc() << endl;
	delete[] data;
	return msg;
}

bool MpiControl::ireceive(int src)
{
	MPI::Status stat;
	MPI::Request req;
	unsigned queueSize = mPriInRequests.size();
	Message* msg = 0;
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
	return !mInQueue.empty();
}

void MpiControl::send(Message* msg)
{
//	cout << mRank << " is sending to " << msg->getDst() << "..." << endl;
	MPI::Status stat;
	if (msg != 0){
//		cout << "sending immediate msg...." << endl;
//		cout << mRank << " to " << msg->getDst() << "" << endl;
		MPI::COMM_WORLD.Ssend(msg->getData(), msg->getLength(), MPI_CHAR, msg->getDst(), msg->getType());
		delete msg;
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

void MpiControl::isend()
{
//	cout << mRank << " is sending to " << msg->getDst() << "..." << endl;
	MPI::Status stat;

	Message* msg = 0;

	if (!mPriOutRequests.empty()){
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
	else if (!mOutQueue.empty()){
		msg = mOutQueue.front();
		mOutQueue.pop();
		msg->request = MPI::COMM_WORLD.Isend(msg->getData(), msg->getLength(), MPI_CHAR,msg->getDst(), msg->getType());
		mPriOutRequests.push(msg);
	}

//	unsigned queueSize = mPriOutRequests.size();
//	for (unsigned i=0; i<queueSize; ++i){
//		msg = mPriOutRequests.front();
//		mPriOutRequests.pop();
//		if (msg->request.Test()){
////			cout << "received " << msg->getType() << " from " << msg->getSrc() << endl;
//			mInQueue.push(msg);
//		}
//		else{
//			mPriInRequests.push(msg);
//		}
//	}
//// ------------------------------
//	if (msg != 0){
//		cout << "isending immediate msg...." << endl;
//		cout << mRank << " to " << msg->getDst() << "" << endl;
//		MPI::COMM_WORLD.Isend(msg->getData(), msg->getLength(), MPI_CHAR, msg->getDst(), msg->getType());
//		delete msg;
//		cout << "deleted msg!" << endl;
//	}
//	else if (!mOutQueue.empty()){
//		Message* msg = mOutQueue.front();
//		MPI::COMM_WORLD.Isend(msg->getData(), msg->getLength(), MPI_CHAR,msg->getDst(), msg->getType());
//		mOutQueue.pop();
//		cout << "isend " << msg->getType() << " to " << msg->getDst() << endl;
//		delete msg;
//	}
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

//void MpiControl::loop(int src, int dst)
//{
//	receive(src);
//	send(dst);
//}

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
	mOutQueue.push(msg);
}

void MpiControl::finalize()
{
	MPI::Finalize();
}

void MpiControl::makeRenderGroup(int size, const int* ranks)
{
	mRenderGroup.Incl(size,ranks);
	mRenderComm.Create(mRenderGroup);

}

void
MpiControl::makeDataGroup(int size, const int* ranks)
{
	mDataGroup.Incl(size,ranks);
	mDataComm.Create(mDataGroup);
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

