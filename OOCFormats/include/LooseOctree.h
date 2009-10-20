/**
 * @file	LooseOctree.h
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 17.05.2009
 * @brief 	LooseOctree class declaration.
 *
 */

#ifndef LOOSEOCTREE_H_
#define LOOSEOCTREE_H_

#include <string>
#include <vector>
#include <list>
#include <set>
#include <queue>


#include "declarations.h"
#include "StructDefs.h"
#include "BoundingBox.h"

#define MAX_OCTREE_LEVEL 14
#define MAX_OCTREE_TRIANGLES 5000

namespace oocformats {

/**
 * @class LooseOctree
 * @brief This is a special kind of octree wich will serve as a pre-step for the
 * generation of the real LooseOctree. It will contain it's normal BoundingBox as
 * well as the extended BoundingBox, which is typical for a loose octree.
 * It will only contain strings each of which contains a path to the object on
 * disc to partitionate work while generating the real loose octree.
 * <a href="http://anteru.net/2008/11/14/315/" target="_blank">An overview of loose octrees.</a>
 *
 */
class LooseOctree
{

	public:
		LooseOctree(LooseOctree* _father, const ooctools::BoundingBox& _bb, int64_t _id = 0);
		LooseOctree(const char* nodeSkel);
		virtual ~LooseOctree();

		/**
		 * @brief True, if this node has no children.
		 */
		virtual bool isLeaf() const;
		virtual bool isRoot() const;

		/**
		 * @brief Evaluates true if the node is leaf-node but it has no data assigned to it.
		 * Hmmm, i'm sure this function would not survive a sanity-check. Either it's a leaf-node,
		 * but then it will have data. If this would ever be true, the node is obsolete!
		 */
		bool isEmpty() const;

		/**
		 * @brief true if either actual data is loaded in the current node or a triCount > 0 is
		 * set in the tree's skeleton.
		 */
		bool hasData() const;

		bool conditionHolds() const;
		unsigned int getLevel() const {return mPriLevel;};
		const virtual ooctools::BoundingBox& getBb() const {return mBb;};
		const ooctools::BoundingBox& getExtBb() const {return mExtBb;};
		std::list<ooctools::Triangle>& getData() {return mTriList;};
		std::size_t getTriangleCount() const;
		int64_t getId() const {return mPriId;};
		std::string getIdString()const;
		float getAreaSum() const {return mPriAreaSum;};

		void getAllSubtreeIds(std::set<uint64_t>* _ids);
		void getAllSubtreeIds(std::set<uint64_t>* _ids, unsigned orderIdx, const ooctools::V3f& eyeDist, const float* distArray);

		/**
		 * @brief Debug-Function to check the correctness of subdivision.
		 * @param maxLevel
		 */
		void generateEmptyTree(unsigned int maxLevel);

		/**
		 * @brief inserts a list of triangles at the current node. This special
		 * method performs no constraint checks or subdivision operations.
		 * It just adds them to the current List
		 * @param triList
		 */
		void insertTriList(std::list<ooctools::Triangle>& triList);
		void insertTriangle(ooctools::Triangle& tri);
		void trimSubTreeData();

		/**
		 * @brief Pushes a list of Triangle into the octree.This causes any existing triangles
		 * to be deleted. This method will not check for the constraint neither will it subdivide
		 * data into lower nodes. It just writes data into the node.
		 */
		void setTriList(std::list<ooctools::Triangle> triList);

		/**
		 * @brief This just adds the triList to the triangles already present in this node.
		 * Note that this will not cause a bubbling down of triangles. It has to be induced manually.
		 */
		void pushTriList(std::vector<ooctools::Triangle>& triList);
		void pushTriangle(ooctools::Triangle& tri);

		/**
		 * @brief Sets the children for this tree. This is not an optimal solution.
		 * Maybe we should implement a set method for a single triangle with a given
		 * child-idx. Or maybe we get the operator[] running in alliance with the
		 * operator= this should be nice. But it needs to be seen whether this goes
		 * along with pointer/reference mixtures.
		 */
		void setChildren(LooseOctree* children[]);

		/**
		 * @brief Inserts a node into the tree. This will bubble down the
		 * node according to it's ID and/or BoundingBox.
		 * This is necessary when loading the tree from disk.
		 * Because we're only saving relevant data-nodes we have to navigate
		 * by ID.
		 * @param node
		 */
		void insertNode(LooseOctree* node);
		void setFather(LooseOctree* father){mFather = father;};
		LooseOctree* getFather() const {return mFather;};
		void getNodeSkeleton(void* nodeSkel);
		static unsigned getSkeletonSize();
		void setLevel(int lvl){mPriLevel = lvl;};
		void setRoot(LooseOctree* _root){mPriRoot = _root;};
		LooseOctree* getChild(unsigned i);
		const LooseOctree* getChild(unsigned i) const;

		/**
		 * @brief Subdivides this node's BoundingBox into the given index-section.
		 * @return the newly calculated BoundingBox.
		 */
		ooctools::BoundingBox subdivideBB(unsigned idx);
		inline LooseOctree* getRoot() const {return mPriRoot;};

		bool makeChild(unsigned int idx);
		void addTriangleArea(double area);
		void incTriCount(unsigned count=1);
		void setTriCount(unsigned count) {mPriTriCount = count;};
		void setAreaSum(double area) {mPriAreaSum = area;};
		void drawLeafBBs();
		unsigned findInsertionPoint(const ooctools::Triangle& tri);
		std::string getDirPrefix(unsigned level = 0) const;
		std::string getChildType() const;
		void printNodePath(int64_t id) const;

		void isInFrustum_bfs(float** _frustum, std::set<uint64_t>* _ids, unsigned orderIdx, unsigned _limit);
		void isInFrustum_dfs(float** _frustum, std::set<uint64_t>* _ids, unsigned orderIdx, unsigned _limit);
		bool frustumSelfTest_bfs(float** _frustum, std::set<uint64_t>* _ids, std::queue<LooseOctree*>& _toDoQueue, unsigned orderIdx);
		bool frustumSelfTest_dfs(float** _frustum, std::set<uint64_t>* _ids, unsigned orderIdx);
		void isInFrustum(float** _frustum, std::set<uint64_t>* _ids, bool _showOctree, unsigned* _threshold, bool debug=false);
		void isInFrustum_orig(float** _frustum, std::set<uint64_t>* _ids, unsigned orderIdx, const ooctools::V3f& eyeDist, const float* distArray);
		void isInFrustum_orig(float** _frustum, std::map<uint64_t, int>* _ids, unsigned orderIdx);
		void setDataLoaded(){mPriDataLoaded = true;};
		void setDataUnloaded(){mPriDataLoaded = false;};
		bool isDataLoaded() const {return mPriDataLoaded;};

		/**
		 * @brief Counts for the whole tree how many children we have
		 * with ID 0, ID 1 etc.
		 */
		static unsigned descendantCount[8];

		/**
		 * @brief Counts the maximum number of triangles in a single node.
		 */
		static unsigned maxTriCount;

		static unsigned totalTriCount;
		static unsigned maxLevel;

		static unsigned orderLUT[26][8];
		static unsigned detailLUT[26][8];

	private:
		typedef std::list<ooctools::Triangle>::const_iterator CTriIter;
		typedef std::list<ooctools::Triangle>::iterator TriIter;
		typedef std::vector<LooseOctree*>::const_iterator CNodeIter;
		typedef std::vector<LooseOctree*>::iterator NodeIter;

		std::list<ooctools::Triangle> mTriList;
		ooctools::BoundingBox mBb;
		ooctools::BoundingBox mExtBb;
		std::string path;

		LooseOctree* mPriRoot;
		int mPriLevel;
		int64_t mPriId;
		unsigned mPriTriCount;
		double mPriAreaSum;
		bool mPriDataLoaded;

		LooseOctree* mChildren[8];
		LooseOctree* mFather;

		const ooctools::BoundingBox extendBb(const ooctools::BoundingBox& _bb) const;
		void calcAreaSum();

		/**
		 * @brief Checks the given ProcessingObject against the central LooseOctre
		 * constraint. See the class documentation for a link to a webpage,
		 * describing this in more detail.
		 * @param _po
		 * @return
		 */
		bool fitsInto(const ooctools::Triangle& _tri) const;

		/**
		 * @brief Creates the complete set of child-nodes. They will be empty, a.k.a.
		 * they will contain no data but are leafes. This step will only be called from
		 * insertObject(), when the constrait isn't met anymore and the data has to
		 * be divided among the child-nodes.
		 */
		void makeChildren();

		void subdivideData();
};

}

#endif /* LOOSEOCTREE_H_ */
