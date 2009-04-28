/**
 * @file	OctreeHandler.h
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 23.03.2009
 * @brief 	OctreeHandler class declaration.
 */

#ifndef OCTREEMODELWRITER_H_
#define OCTREEMODELWRITER_H_

#include "boost/filesystem.hpp"
#include <boost/filesystem/fstream.hpp>

#include "declarations.h"
#include "RawModelHandler.h"
#include "Octree.h"
#include "LooseProcessingOctree.h"

namespace fs = boost::filesystem;
using namespace ooctools;
namespace oocformats {

/**
 * @class OctreeHandler
 * @brief Central point where all external octree-functions are collected.
 * Like generating, saving, loading, etc.
 */
class OctreeHandler
{
	public:
		OctreeHandler();
		virtual ~OctreeHandler();

		void generateOctree(const fs::path& src, const fs::path& dst, const ooctools::BoundingBox& _bb);

		/**
		 * @brief Writes a VBO-Ref to disc in the octree-raw-format
		 * @param _vbo what
		 * @param _path where
		 */
		static void writeOctreeVbo(const ooctools::Vbo& _vbo, fs::path _path);

		/**
		 * @brief Reads a VBO-Ref from disc in the octree-raw-format and returns a pointer
		 * to the object.
		 * @param _path Read from where
		 * @return the Vbo pointer
		 */
		static ooctools::Vbo* readOctreeVbo(fs::path _path);

		/**
		 * @brief Starts recursive depth-first-search at given location and inserts all VBOs
		 * it can find into the VBOManager.
		 * @param _path
		 * @param tree
		 * @todo rewrite it, so that the tree-parameter is "complete" after reading.
		 */
		void readOctreeRecursive(fs::path _path, ooctools::Octree* tree);

		void readLooseProcOctreeRecursive(fs::path _path, LooseProcessingOctree* tree);
		LooseProcessingOctree* rawToProcOctree(const fs::path& _path, const BoundingBox& _bb);
		void writeProcTree(LooseProcessingOctree* _tree);

	private:
		void parsePhase1(const fs::path& src, const fs::path& dst);
		RawModelHandler mPriRawHandler;
		ooctools::Octree* mPriOctree;
		ooctools::LooseProcessingOctree* mProcOctree;
};

} // oocformats

#endif /* OCTREEMODELWRITER_H_ */
