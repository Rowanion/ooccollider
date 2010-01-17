/**
 * @file	OctreeHandler.h
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 23.03.2009
 * @brief 	OctreeHandler class declaration.
 */

#ifndef OCTREEMODELWRITER_H_
#define OCTREEMODELWRITER_H_

#include <string>
#include <list>

#include "boost/filesystem.hpp"
#include <boost/filesystem/fstream.hpp>

#include "declarations.h"
#include "RawModelHandler.h"

#include "LooseProcessingOctree.h"
#include "FileIO.h"
#include "StructDefs.h"
#include "LooseRenderOctree.h"

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
		void writeOctreeVbo(const ooctools::Vbo& _vbo, fs::path _path);

		/**
		 * @brief Reads a VBO-Ref from disc in the octree-raw-format and returns a pointer
		 * to the object.
		 * @param _path Read from where
		 * @return the Vbo pointer
		 */
		ooctools::Vbo* readOctreeVbo(fs::path _path);

		void readLooseProcOctreeRecursive(fs::path _path, LooseProcessingOctree* tree);
		LooseProcessingOctree* rawToProcOctree(const fs::path& _path, const BoundingBox& _bb);
		void writeProcTree(LooseProcessingOctree* _tree);
		bool readProcTreeCell(fs::path path);
		bool readProcTreeCell(fs::path path, std::list<Triangle>& triList);
		bool readLooseOctreeCell(fs::path path, std::string id, std::list<Triangle>& triList);
		bool readLooseOctreeCell(fs::path path, std::string id);
		bool writeLooseOctreeCell(fs::path path, std::string id, const std::list<Triangle>& triList, const BoundingBox& bb);

		bool readSubProcTree(fs::path path);
		bool readSingleLevelProcTree(fs::path path, unsigned int level, unsigned int currentLevel = 0);
		void lpo2lo(std::string lpoSrc, std::string loDst, const BoundingBox& scBb);
		void recursiveLpo2lo(fs::path lpoSrc, fs::path loDst, LooseOctree* lo);
		void saveLooseSubTreeData(LooseOctree* lo, fs::path path);
		void saveLooseOctreeSkeleton(LooseOctree* lo, fs::ofstream& _of);
		LooseOctree* loadLooseOctreeSkeleton(fs::path file);
		LooseRenderOctree* loadLooseRenderOctreeSkeleton(fs::path file);
		void convertProctreeCell(fs::path lpoCell, fs::path dstDir, std::string id, LooseOctree* lo=0);
		void recursiveProc2Octree(fs::path lpoSrc, fs::path dstDir, LooseOctree* lo);
		void recursiveOct2LooseOct(fs::path ocPath, fs::path lOcPath, LooseOctree* lo);
		bool loadSubtreeData(int64_t id, LooseOctree* node, bool foundNode=false);
		void updateSkeleton(LooseOctree* node, fs::path path);

		/**
		 * @brief read tricount from datafiles and set the count inside the given skeleton to it.
		 * @return
		 */
		double calcAreaSumFromPath(fs::path path, std::string id);

		void generateIdPathMap(const LooseOctree* lo, std::map<uint64_t, std::string>& idPathMap) const;
		void generateIdPathMap(const LooseRenderOctree* lo, std::map<uint64_t, std::string>& idPathMap) const;
		void generateIdLocMap(fs::path _path, std::map<uint64_t, ooctools::Location>& _idLocMap) const;
		void generateIdLoMap(LooseOctree* lo, std::map<uint64_t, oocformats::LooseOctree*>& idLoMap) const;
		void generateIdLoMap(LooseRenderOctree* lo, std::map<uint64_t, oocformats::LooseRenderOctree*>& idLoMap) const;

	private:
		void parsePhase1(const fs::path& src, const fs::path& dst);
		char* mapFile(fs::path _path, unsigned _fileSize, int& _fHandle) const;
		void umapFile(char* _map, unsigned _fileSize, int& _fHandle) const;
		RawModelHandler mPriRawHandler;
		LooseProcessingOctree* mProcOctree;
		std::map<std::string, unsigned> codeMap;
		FileIO mFio;
		RawModelHandler rmh;
};

} // oocformats

#endif /* OCTREEMODELWRITER_H_ */
