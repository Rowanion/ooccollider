/**
 * @file	LooseProcessingOctree.h
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 23.04.2009
 *
 */

#ifndef LOOSEPROCESSINGOCTREE_H_
#define LOOSEPROCESSINGOCTREE_H_

#include <string>
#include <vector>

#include "declarations.h"
#include "BoundingBox.h"
#include "OctreeNode.h"

#define MAX_PROCESSING_OCTREE_LEVEL 16
#define MAX_PROCESSING_OCTREE_VERTICES 1000

namespace ooctools {

/**
 * @class LooseProcessingOctree
 * @brief This is a special kind of octree wich will serve as a pre-step for the
 * generation of the real LooseOctree. It will contain it's normal BoundingBox as
 * well as the extended BoundingBox, which is typical for a loose octree.
 * It will only contain strings each of which contains a path to the object on
 * disc to partitionate work while generating the real loose octree.
 * <a href="http://anteru.net/2008/11/14/315/" target="_blank">An overview of loose octrees.</a>
 *
 */
class LooseProcessingOctree
{

	public:
		LooseProcessingOctree(LooseProcessingOctree* _father, const BoundingBox& _bb, std::string _path);
		virtual ~LooseProcessingOctree();

		/**
		 * @brief True, if this node has no children.
		 * @return
		 */
		virtual bool isLeaf();
		virtual bool isRoot();
		bool isEmpty();
		virtual BoundingBox subdivideFne();
		virtual BoundingBox subdivideFnw();
		virtual BoundingBox subdivideFse();
		virtual BoundingBox subdivideFsw();
		virtual BoundingBox subdivideBne();
		virtual BoundingBox subdivideBnw();
		virtual BoundingBox subdivideBse();
		virtual BoundingBox subdivideBsw();
		LooseProcessingOctree* getFne() const {return fne;};
		LooseProcessingOctree* getFnw() const {return fnw;};
		LooseProcessingOctree* getFse() const {return fse;};
		LooseProcessingOctree* getFsw() const {return fsw;};
		LooseProcessingOctree* getBne() const {return bne;};
		LooseProcessingOctree* getBnw() const {return bnw;};
		LooseProcessingOctree* getBse() const {return bse;};
		LooseProcessingOctree* getBsw() const {return bsw;};

		/**
		 * @brief As the name suggests, this inserts a object into the tree.
		 * This method uses recursion.
		 * @param _po An instance of the struct ProcessingObject defined in StructDefs.h.
		 * @return true, if the object was inserted properly.
		 */
		bool insertObject(ProcessingObject* _po);
		unsigned char calculateLevel(unsigned char counter);
		bool conditionHolds(unsigned int addOn);
		unsigned int getLevel() const {return mPriLevel;};
		const virtual BoundingBox& getBb() const {return mBb;};
		const BoundingBox& getExtBb() const {return mExtBb;};
		const std::string& getPath() const {return path;};
		void setBasePath(const std::string& _path, int oldSize = -1);
		std::vector<ProcessingObject*>& getData() {return mObjectList;};
		unsigned int getTriangleCount() const {return nTriangles;};

	private:
		std::vector<ProcessingObject*> mObjectList;
		BoundingBox mBb;
		BoundingBox mExtBb;
		std::string path;

		unsigned int nTriangles;
		LooseProcessingOctree* mPriRoot;
		unsigned char mPriLevel;

		LooseProcessingOctree* father;
		LooseProcessingOctree* fne;
		LooseProcessingOctree* fnw;
		LooseProcessingOctree* fse;
		LooseProcessingOctree* fsw;
		LooseProcessingOctree* bne;
		LooseProcessingOctree* bnw;
		LooseProcessingOctree* bse;
		LooseProcessingOctree* bsw;

		const BoundingBox extendBb(const BoundingBox& _bb) const;
		LooseProcessingOctree* getRoot() const {return mPriRoot;};

		/**
		 * @brief Checks the given ProcessingObject against the central LooseOctre
		 * constraint. See the class documentation for a link to a webpage,
		 * describing this in more detail.
		 * @param _po
		 * @return
		 */
		bool fitsInto(ProcessingObject* _po) const;

		/**
		 * @brief Creates the complete set of child-nodes. They will be empty, a.k.a.
		 * they will contain no data but are leafes. This step will only be called from
		 * insertObject(), when the constrait isn't met anymore and the data has to
		 * be divided among the child-nodes.
		 */
		void makeChildren();
};

}

#endif /* LOOSEPROCESSINGOCTREE_H_ */
