/**
 * @file	Octree.h
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 13.04.2009
 * @brief 	Octree class declaration.
 */

#ifndef OCTREE_H_
#define OCTREE_H_

#include <string>

#include "declarations.h"
#include "OctreeNode.h"
#include "BoundingBox.h"

namespace ooctools {

/**
 * @class Octree
 * @brief Root-Node representation of the octree.
 * Note that this special octree is not meant to be generated on the fly, but is being
 * parsed object by object and is generated on disk. Be prepared the have a large HDD ready!
 */
class Octree: public ooctools::OctreeNode
{
	public:
		Octree(const BoundingBox& sBb, std::string _path);
		virtual ~Octree();
		void parseSceneBb();
		void setSceneBb(const BoundingBox& _bb){mPriBb = _bb;};

	private:
};

}

#endif /* OCTREE_H_ */
