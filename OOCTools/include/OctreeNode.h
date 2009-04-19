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

#include "BoundingBox.h"
#include "declarations.h"
#include "Vbo.h"

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
		OctreeNode(OctreeNode* _father, const BoundingBox& _bb, std::string _path);
		virtual ~OctreeNode();

		/**
		 * @brief True, if this node has no children.
		 * @return
		 */
		virtual bool isLeaf();
		virtual bool isRoot();
		BoundingBox subdivideFne();
		BoundingBox subdivideFnw();
		BoundingBox subdivideFse();
		BoundingBox subdivideFsw();
		BoundingBox subdivideBne();
		BoundingBox subdivideBnw();
		BoundingBox subdivideBse();
		BoundingBox subdivideBsw();
		const BoundingBox& getBb() const {return mPriBb;};

		/**
		 * @warning UNTESTED!
		 * @param _vbo
		 * @return
		 */
		bool insertVbo(ooctools::Vbo& _vbo);

		// if is maxlevel -> load vbo there, if any, merge, save, delete vbos.
		// else if is leaf -> load vbo there, if any, merge, test against constraint...
		//		if holds, save, delete vbos.
		//		else subdivide, propagate to children, delete old file on save

	protected:
		BoundingBox mPriBb;
		size_t nTriangles;
		size_t nVertices;
		std::string path;
		Octree* mPriRoot;
		unsigned char mPriLevel;
		const unsigned int mPriMaxLevel; // <- maxlevel = 8
		unsigned char calculateLevel(unsigned char counter = 0);
		bool testCondition(const Vbo& _vbo);

		/**
		 * @warning UNTESTED!
		 * @param _vbo
		 * @return
		 */
		bool subdivide(ooctools::Vbo& _vbo);


		OctreeNode* father;
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
