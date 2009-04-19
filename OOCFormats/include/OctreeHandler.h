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
#include "RawModelWriter.h"
#include "Octree.h"

namespace fs = boost::filesystem;
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

		void readOctreeRecursive(fs::path _path);

	private:
		void parsePhase1(const fs::path& src, const fs::path& dst);
		RawModelWriter mPriRawHandler;
		ooctools::Octree* mPriOctree;
};

} // oocformats

#endif /* OCTREEMODELWRITER_H_ */
