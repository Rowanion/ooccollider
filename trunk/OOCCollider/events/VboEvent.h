/**
 * @file	VboEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	VboEvent class declaration.
 */

#ifndef VBOEVENT_H_
#define VBOEVENT_H_

#include <vector>
#include <string>

#include "IEvent.h"
#include "ClassId.h"
#include "Message.h"
#include "IndexedVbo.h"
#include "StructDefs.h"

namespace fs = boost::filesystem;

/**
 * @class VboEvent
 *
 * @brief This event is actually a container event for packaging, shipping and receiving the modeldata
 * over the network.
 *
 * This event originates from Data-Nodes and will be sent to Render-Nodes.
 * This event is a reply to NodeRequestEvent.
 *
 */
class VboEvent : public oocframework::IEvent{
public:
	VboEvent();
	VboEvent(const ooctools::IndexedVbo* vbo, uint64_t nodeId);
	VboEvent(std::string path, uint64_t nodeId);
	VboEvent(const std::vector<ooctools::IndexedVbo*>& vboVec);
	VboEvent(const Message* msg);
	virtual ~VboEvent();
	static const oocframework::ClassId* classid();
	inline virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

	/**
	 * @brief Switching to member for byteSize because the size may differ per event.
	 * The resolution is the key to size determination.
	 * @return the size in bytes stored in this particular events' member mPriData.
	 */
	inline virtual unsigned getByteSize(){return mPriByteSize;};

	/**
	 * @brief returns the Octree-node-Id to which this VBO belongs.
	 */
	inline uint64_t getNodeId() const {return ((uint64_t*)mProData)[0];};

	/**
	 * @brief returns the number of indices stored in this vbo.
	 * Obvious side-node: @f$ \frac{\# indices}{3} = \# triangles @f$
	 */
	inline unsigned getIndexCount() const {return ((unsigned*)(mProData+sizeof(uint64_t)))[0];};

	/**
	 * @brief returns the number of <strong>vertices</strong> stored in this vbo.
	 * Remember that in our case a <strong>vertex</strong> consists of a 4-float-component 3D-Position,
	 * and a 4-char-component vertex-normal.
	 * @see ooctools::V4N4
	 */
	inline unsigned getVertexCount() const {return ((unsigned*)(mProData+sizeof(uint64_t)))[1];};

	/**
	 * @brief Returns a const pointer to the index-data of the vbo.
	 */
	inline const unsigned* getIndexArray() const {
		return ((unsigned*)(mProData+sizeof(uint64_t)+sizeof(unsigned)*2));
	};

	/**
	 * @brief Returns a const pointer to the vertex-data of the vbo.
	 */
	inline const ooctools::V4N4* getVertexArray() const {
		return ((const ooctools::V4N4*)(mProData+sizeof(uint64_t)+sizeof(unsigned)*2 + sizeof(unsigned)*getIndexCount()));
	};

	/**
	 * @brief Creates a packed-up-and-ready-to-go IndexedVbo and returns it.
	 * @note The instance is created via the 'new' operand. The receiver has to
	 * to do all the cleaning-up.
	 */
	ooctools::IndexedVbo* createIVbo() const;

	// ------------------------------------------------------------------------------------------
	//TODO after done implementation of new methods add a '=0' default val to the params
	const unsigned* getIndexArray(unsigned idx) const;
	unsigned getVboCount() const;
	unsigned getBytePrefixSum(unsigned idx) const;
	unsigned getIndexCount(unsigned idx) const;
	unsigned getVertexCount(unsigned idx) const;
	uint64_t getNodeId(unsigned idx) const;



protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
	unsigned mPriByteSize;
};

#endif /* VBOEVENT_H_ */
