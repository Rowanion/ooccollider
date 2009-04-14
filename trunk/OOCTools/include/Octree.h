/**
 * @file	Octree.h
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 13.04.2009
 * @brief 	Octree class declaration.
 */

#ifndef OCTREE_H_
#define OCTREE_H_

#include "OctreeNode.h"

namespace ooctools {

/**
 * @class Octree
 * @brief Root-Node representation of the octree.
 */
class Octree: public virtual ooctools::OctreeNode
{
	public:
		Octree();
		virtual ~Octree();
};

}

#endif /* OCTREE_H_ */
