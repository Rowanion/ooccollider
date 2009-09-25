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


class MpiControl {
public:

	enum Group{
		DEFAULT = -1,
		ALL = 0,
		RENDERER = 1,
		DATA = 2
	};
	virtual ~MpiControl();
	void init();

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
	void completeWaitingReceives(const oocframework::ClassId* classid);
	void send(Message* msg = 0);
	void isend();
	void sendAll();
//	void sendImmediate(Message* msg);
//	void sendImmediateToAll(Message* msg);
	void finalize();
	inline bool inQueueEmpty() const {return (mInQueue.empty());};
	inline bool outQueueEmpty() const {return (mOutQueue.empty());};
	inline unsigned getInQueueSize() const {return (mInQueue.size());};
	inline unsigned getOutQueueSize() const {return (mOutQueue.size());};

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

	inline MPI::Group& getGlobalGrp() {return mOrigGroup;};
	inline MPI::Group& getRenderGrp() {return mRenderGroup;};
	inline MPI::Group& getDataGrp() {return mDataGroup;};

	inline Group getGroup() const {return mGroup;};


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

	static MpiControl* instance;

};

#endif /* MPICONTROL_H_ */
