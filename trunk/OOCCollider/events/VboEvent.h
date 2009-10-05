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

#include "declarations.h"
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
 * @warning This class is not safe when created with vector of size 0. Will throw segfaults when accessing
 * fields or methods.
 *
 */
class VboEvent : public oocframework::IEvent{
public:
	VboEvent();
	VboEvent(const ooctools::IndexedVbo* vbo);
	VboEvent(std::string path, uint64_t nodeId);
	VboEvent(const std::vector<ooctools::IndexedVbo*>& vboVec, const std::vector<float>& distVec, bool isExtFrustum);
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
	 * @brief Returns the number of VBOs encapsulated in this event.
	 */
	unsigned getVboCount() const;

	/**
	 * @brief returns the Octree-node-Id to which this VBO belongs.
	 */
	uint64_t getNodeId(unsigned idx=0) const;

	/**
	 * @brief returns the number of indices stored in this vbo.
	 * Obvious side-node: @f$ \frac{\# indices}{3} = \# triangles @f$
	 */
	unsigned getIndexCount(unsigned idx=0) const;

	/**
	 * @brief returns the number of <strong>vertices</strong> stored in this vbo.
	 * Remember that in our case a <strong>vertex</strong> consists of a 4-float-component 3D-Position,
	 * and a 4-char-component vertex-normal.
	 * @see ooctools::V4N4
	 */
	unsigned getVertexCount(unsigned idx=0) const;

	/**
	 * @brief Returns a const pointer to the index-data of the vbo.
	 */
	const unsigned* getIndexArray(unsigned idx=0) const;

	/**
	 * @brief Returns a const pointer to the vertex-data of the vbo.
	 */
	const ooctools::V4N4* getVertexArray(unsigned idx=0) const;

	/**
	 * @brief Creates a packed-up-and-ready-to-go IndexedVbo and returns it.
	 * @note The instance is created via the 'new' operand. The receiver has to
	 * to do all the cleaning-up.
	 */
	ooctools::IndexedVbo* createIVbo() const;

	float getDist(unsigned idx) const;

	/**
	 * @brief Returns whether the included VBOs are from extended frustum or not (ie. the original frustum).
	 */
	bool isExtendedFrustum() const;

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
	unsigned mPriByteSize;

	/**
	 * @brief Returns the number of bytes all other VBOs prior to this one take up.
	 * Because every VBO can have its own individual size, we need to jump irregular within our
	 * data container. The VBO with index 0 would need to jump 0 bytes, because there is no VBO prior
	 * to that one. The VBO with index 10 would need to jump the accumulated number of bytes all of its
	 * predecessors take up. And that's what this function returns.
	 * Because of this fact, the bytePrefixSums array has vboCount+1 fields, the first entry always contains
	 * 0 and the last entry always contains the sum of all VBOs.
	 */
	unsigned getBytePrefixSum(unsigned idx=0) const;
};

#endif /* VBOEVENT_H_ */
