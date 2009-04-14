/**
 * @file	OctreeNode.h
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 13.04.2009
 * @brief 	OctreeNode class declaration.
 *
 */

#ifndef OCTREENODE_H_
#define OCTREENODE_H_

#include <string>

#include "declarations.h"

namespace ooctools {

/**
 * @class OctreeNode
 * @brief Represents a node in an octree datastructure.
 * It contains everything you need to know about your current octree node:
 * <ul>
 * <li>number of vertices contained in this sub-tree</li>
 * <li>number of triangles contained in this subtree</li>
 * <li>references to the eight children, if any</li>
 * <li>reference to tree-root</li>
 * <li>a BoundingBox which represents the sub-tree</li>
 * <li>string containing its /-delimited path from the root to here, because we safe
 * the octree to disk; and thus we can easily convert it to a boost::filesystem::path</li>
 * </ul>
 * @see Vbo, VboManager, BoundingBox
 */
class OctreeNode
{
	public:
		OctreeNode();
		virtual ~OctreeNode();
		bool isLeaf();

	protected:
		BoundingBox mPriBb;
		size_t nTriangles;
		size_t nVertices;
		std::string path;
		Octree* mPriRoot;

		OctreeNode* fne;
		OctreeNode* fnw;
		OctreeNode* fse;
		OctreeNode* fsw;
		OctreeNode* bne;
		OctreeNode* bnw;
		OctreeNode* bse;
		OctreeNode* bsw;
};

}

#endif /* OCTREENODE_H_ */
