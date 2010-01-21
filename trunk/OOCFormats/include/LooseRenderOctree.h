/**
 * @file	LooseRenderOctree.h
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 17.05.2009
 * @brief 	LooseRenderOctree class declaration.
 *
 */

#ifndef LOOSERENDEROCTREE_H_
#define LOOSERENDEROCTREE_H_

#include <string>
#include <vector>
#include <list>
#include <set>
#include <queue>


#include "declarations.h"
#include "StructDefs.h"
#include "BoundingBox.h"
#include "IndexedVbo.h"

#define MAX_RENDER_OCTREE_LEVEL 14
#define MAX_RENDER_OCTREE_TRIANGLES 5000

namespace oocformats {

struct WrappedOcNode{
	enum State{
		REQUESTED = 1,
		ONLINE = 2,
		MISSING = 3,
		OFFLINE = 4,
		RETEST_OFFLINE = 5,
		RETEST_ONLINE = 6,
		REQUESTED_OFFLINE = 7,
		REQUESTED_ONLINE = 8
	};

	WrappedOcNode()
	{
		octreeNode = 0;
		iVbo = 0;
		dist = 0.0;
		state = MISSING;
		timeStamp = 0;
		usageCount = 0;
		offlineCount = 0;
	}

	WrappedOcNode(LooseRenderOctree* _octreeNode, ooctools::IVbo* _iVbo, float _dist, State _state, unsigned _time, unsigned _usageCount)
	{
		octreeNode = _octreeNode;
		iVbo = _iVbo;
		dist = _dist;
		state = _state;
		timeStamp = _time;
		usageCount = _usageCount;
	}

	WrappedOcNode(LooseRenderOctree* _octreeNode)
	{
		octreeNode = _octreeNode;
		iVbo = 0;
		dist = 0.0;
		state = MISSING;
		timeStamp = 0;
		usageCount = 0;
		offlineCount = 0;
	}

	bool operator<(const WrappedOcNode& rhs) const
	{
		if (timeStamp == rhs.timeStamp){
			if (dist == rhs.dist){
				return (octreeNode > rhs.octreeNode);
			}
			else {
				return (dist > rhs.dist);
			}
		}
		else {
			return (timeStamp > rhs.timeStamp);
		}
	}

	LooseRenderOctree* octreeNode;
	ooctools::IVbo* iVbo;
	State state;
	float dist;
	unsigned timeStamp;
	unsigned usageCount;
	unsigned offlineCount;
	unsigned dummy;
};


/**
 * @class LooseRenderOctree
 * @brief This is a special kind of octree wich will serve as a pre-step for the
 * generation of the real LooseRenderOctree. It will contain it's normal BoundingBox as
 * well as the extended BoundingBox, which is typical for a loose octree.
 * It will only contain strings each of which contains a path to the object on
 * disc to partitionate work while generating the real loose octree.
 * <a href="http://anteru.net/2008/11/14/315/" target="_blank">An overview of loose octrees.</a>
 *
 */
class LooseRenderOctree
{

	public:
		LooseRenderOctree(LooseRenderOctree* _father, const ooctools::BoundingBox& _bb, int64_t _id = 0);
		LooseRenderOctree(const char* nodeSkel);
		virtual ~LooseRenderOctree();

		/**
		 * @brief True, if this node has no children.
		 */
		bool isLeaf() const;
		bool isRoot() const;

		/**
		 * @brief Evaluates true if the node is leaf-node but it has no data assigned to it.
		 * Hmmm, i'm sure this function would not survive a sanity-check. Either it's a leaf-node,
		 * but then it will have data. If this would ever be true, the node is obsolete!
		 */
		bool isEmpty() const;

		unsigned int getLevel() const {return mPriLevel;};
		const ooctools::BoundingBox& getBb() const {return mBb;};
		const ooctools::BoundingBox& getExtBb() const {return mExtBb;};
		std::size_t getTriangleCount() const;
		int64_t getId() const {return mPriId;};
		std::string getIdString()const;

		void getAllSubtreeIds(std::list<WrappedOcNode*>* _nodes, bool _isExt, unsigned& listSize, unsigned maxSize);
		void getAllSubtreeIds(std::list<WrappedOcNode*>* _nodes, unsigned orderIdx, const ooctools::V3f& eyePos, const float* distArray, bool _isExt, unsigned& listSize, unsigned maxSize);

		/**
		 * @brief Debug-Function to check the correctness of subdivision.
		 * @param maxLevel
		 */
		void generateEmptyTree(unsigned int maxLevel);

		/**
		 * @brief Sets the children for this tree. This is not an optimal solution.
		 * Maybe we should implement a set method for a single triangle with a given
		 * child-idx. Or maybe we get the operator[] running in alliance with the
		 * operator= this should be nice. But it needs to be seen whether this goes
		 * along with pointer/reference mixtures.
		 */
		void setChildren(LooseRenderOctree* children[]);

		/**
		 * @brief Inserts a node into the tree. This will bubble down the
		 * node according to it's ID and/or BoundingBox.
		 * This is necessary when loading the tree from disk.
		 * Because we're only saving relevant data-nodes we have to navigate
		 * by ID.
		 * @param node
		 */
		void insertNode(LooseRenderOctree* node);
		void setFather(LooseRenderOctree* father){mFather = father;};
		LooseRenderOctree* getFather() const {return mFather;};
		void getNodeSkeleton(void* nodeSkel);
		static unsigned getSkeletonSize();
		void setLevel(int lvl){mPriLevel = lvl;};
		LooseRenderOctree* getChild(unsigned i);
		const LooseRenderOctree* getChild(unsigned i) const;
		bool hasData() const;
		/**
		 * @brief Subdivides this node's BoundingBox into the given index-section.
		 * @return the newly calculated BoundingBox.
		 */
		ooctools::BoundingBox subdivideBB(unsigned idx);

		bool makeChild(unsigned int idx);
		void incTriCount(unsigned count=1);
		void setTriCount(unsigned count) {mPriTriCount = count;};
		void drawLeafBBs();
		std::string getDirPrefix(unsigned level = 0) const;
		std::string getChildType() const;
		void printNodePath(int64_t id) const;

		void isInFrustum_bfs(float** _frustum, std::set<uint64_t>* _ids, unsigned orderIdx, unsigned _limit);
		void isInFrustum_dfs(float** _frustum, std::set<uint64_t>* _ids, unsigned orderIdx, unsigned _limit);
		bool frustumSelfTest_bfs(float** _frustum, std::set<uint64_t>* _ids, std::queue<LooseRenderOctree*>& _toDoQueue, unsigned orderIdx);
		bool frustumSelfTest_dfs(float** _frustum, std::set<uint64_t>* _ids, unsigned orderIdx);
		void isInFrustum(float** _frustum, std::set<uint64_t>* _ids, bool _showOctree, unsigned* _threshold, bool debug=false);
		void isInFrustum_orig(float** _frustum, std::list<WrappedOcNode*>* _nodes, unsigned orderIdx, const ooctools::V3f& eyePos, const float* distArray, bool _isExt, unsigned& listSize, unsigned maxSize);

		WrappedOcNode* getWrapper() {return &mPriWrapper;};

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

		static unsigned pubTick;
		static unsigned pubTickLimit;
		static unsigned int pubNodeCount;
		static LooseRenderOctree* pubRoot;

	private:
		typedef std::list<ooctools::Triangle>::const_iterator CTriIter;
		typedef std::list<ooctools::Triangle>::iterator TriIter;
		typedef std::vector<LooseRenderOctree*>::const_iterator CNodeIter;
		typedef std::vector<LooseRenderOctree*>::iterator NodeIter;

		ooctools::BoundingBox mBb;
		ooctools::BoundingBox mExtBb;
		std::string path;
		int64_t mPriId;
		WrappedOcNode mPriWrapper;

		//TODO change to byte
		int mPriDistanceUpdateKey;
		int mPriLevel;
		static unsigned treeTriCount;
		static unsigned* triLimits;
		static bool firstCall;

		//TODO change to LooseRenderOctree**
		LooseRenderOctree* mChildren[8];
		LooseRenderOctree* mFather;
		unsigned mPriTriCount;

		const ooctools::BoundingBox extendBb(const ooctools::BoundingBox& _bb) const;

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

#endif /* LOOSERENDEROCTREE_H_ */
