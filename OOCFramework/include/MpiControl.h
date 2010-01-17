/**
 * @file	MpiControl.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 07.05.2009
 * @brief 	MpiControl class declaration.
 */

#ifndef MPICONTROL_H_
#define MPICONTROL_H_

#include "mpi.h"

#include <queue>

#include "declarations.h"
#include "ClassId.h"

#define MSG_KILL 1
#define MSG_WINDOW_RESIZE 2
#define MSG_FRUSTUM_RESIZE 3

namespace oocframework{

class MpiControl {
public:

	enum Group{
		DEFAULT = -1,
		ALL = 0,
		RENDERER = 1,
		DATA = 2,
		ANY = 3
	};
	virtual ~MpiControl();
	void init(int argc, char *argv[]);

	int getRank() const {return mRank;};
	int getSize() const {return mSize;};
	Message* pop();
	void push(Message* msg);

	static MpiControl* getSingleton();
	void receive(int src);
	void receive(Group _group);
	bool ireceive(int src);
	void ireceive(Group _group);
	Message* directReceive(const oocframework::ClassId* classid);
	bool probe(int src, const oocframework::ClassId* _classid = 0);

	void completeWaitingReceives(const oocframework::ClassId* classid = 0);
	void send(Message* msg = 0);
	void isend(Message* msg = 0);
	void sendAll();
//	void sendImmediate(Message* msg);
//	void sendImmediateToAll(Message* msg);
	void finalize();
	bool inQueueEmpty() const {return (mInQueue.empty());};
	bool outQueueEmpty() const {return (mOutQueue.empty());};
	bool inRequestsEmpty() const {return (mPriInRequests.empty());};
	bool outRequestsEmpty() const {return (mPriOutRequests.empty());};
	unsigned getInQueueSize() const {return (mInQueue.size());};
	unsigned getOutQueueSize() const {return (mOutQueue.size());};

	void makeRenderGroup(int size, const int* ranks);
	void makeDataGroup(int size, const int* ranks);
	void debug();
	void clearOutQueue(int dst = -1);
	void clearOutQueue(Group _group);
	void clearInQueue(int src = -1);
	void clearInQueue(Group _group);
	unsigned getGroupSize(Group _group) const;
	const std::vector<int>& getRenderGroup() const;
	const std::vector<int>& getDataGroup() const;

	MPI::Group& getGlobalGrp() {return mOrigGroup;};
	MPI::Group& getRenderGrp() {return mRenderGroup;};
	MPI::Group& getDataGrp() {return mDataGroup;};

	Group getGroup() const {return mGroup;};
	void barrier() const {MPI_Barrier(MPI::COMM_WORLD);};
	void iCheck();


private:
	MpiControl();

	int mRank;
	int mSize;
	Group mGroup;
	int mGrpRank;
	int mGrpSize;
	MPI::Group mOrigGroup, mRenderGroup, mDataGroup;
	MPI::Intracomm mRenderComm, mDataComm;

	std::vector<int> mPriRenderNodes;
	std::vector<int> mPriDataNodes;

	std::queue<Message*> mInQueue;
	std::queue<Message*> mOutQueue;

	std::queue<Message*> mPriInRequests;
	std::queue<Message*> mPriOutRequests;

	unsigned mPriMaxMessageSize;

	static MpiControl* instance;

};

} // oocframework

#endif /* MPICONTROL_H_ */
