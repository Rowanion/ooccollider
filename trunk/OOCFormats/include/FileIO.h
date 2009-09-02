/**
 * @file	FileIO.h
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 16.04.2009
 * @brief   Class declaration of FileIO
 */

#ifndef FILEIO_H_
#define FILEIO_H_

#include <cctype>
#include <list>

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "declarations.h"
#include "../../OOCTools/include/declarations.h"

namespace fs = boost::filesystem;
namespace oocformats {

/**
 * @class FileIO
 * @brief What a surprise - predefined FileIO is found here ..ooO( x-o ).
 * This is a collection of mostly static methods to encapsulate lowlevel file i/o.
 */
class FileIO
{
	public:
		FileIO();
		virtual ~FileIO();
		void writeV3f(const ooctools::V3f& _v, fs::ofstream& _of);
		void writeV3f(const ooctools::V3f& _v, fs::fstream& _of);
		void readV3f(ooctools::V3f& v, fs::ifstream& _if);
		void readV3f(ooctools::V3f& v, fs::fstream& _if);
		void writeV4f(const ooctools::V4f& _v, fs::ofstream& _of);
		ooctools::V4f readV4f(fs::ifstream& _if);
		void writeV3ub(ooctools::V3ub* _v, fs::ofstream& _of);
		void writeV3ub(ooctools::V3ub* _v, fs::fstream& _of);
		ooctools::V3ub* readV3ub(fs::ifstream& _if);
		ooctools::V3ub* readV3ub(fs::fstream& _if);
		void writeV3b(ooctools::V3b* _v, fs::ofstream& _of);
		void writeBB(ooctools::BoundingBox* _bb, fs::ofstream& _of);
		void writeBB(ooctools::BoundingBox* _bb, fs::fstream& _of);
		void writeVArrayf(const ooctools::VertexArray<float>* _va, fs::ofstream& _of);
		void writeVArrayb(const ooctools::VertexArray<char>* _va, fs::ofstream& _of);
		void writeInt(int _i, fs::ofstream& _of);
		void writeInt(int _i, fs::fstream& _of);
		void writeUInt(unsigned int _i, fs::ofstream& _of);
		void writeUInt(unsigned int _i, fs::fstream& _of);
		void writeFloat(float _f, fs::ofstream& _of);
		char readByte(fs::ifstream& _if);
		unsigned char readUByte(fs::ifstream& _if);
		int readInt(fs::ifstream& _if);
		int readInt(fs::fstream& _if);
		unsigned int readUInt(fs::ifstream& _if);
		unsigned int readUInt(fs::fstream& _if);
		float readFloat(fs::ifstream& _if);
		void writeByte(char _b, fs::ofstream& _of);
		void writeUByte(unsigned char _ub, fs::ofstream& _of);
		float* readFloatArray(fs::ifstream& _if, unsigned count);
		char* readByteArray(fs::ifstream &_if, unsigned count);
		ooctools::BoundingBox* readBB(fs::ifstream& _if);
		ooctools::BoundingBox* readBB(fs::fstream& _if);
		void readHeader(FileHeader& fh, fs::ifstream& _if);
		void readHeader(FileHeader& fh, fs::fstream& _if);
		void writeHeader(ooctools::MetaGroup* _grp, fs::ofstream& _of);
		void writeHeader(FileHeader& _header, fs::ofstream& _of);
		void writeHeader(FileHeader& _header, fs::fstream& _of);
		void writeTriList(const std::list<ooctools::Triangle>& _triList, fs::ofstream& _vFile, fs::ofstream& _nFile);
		void writeTriangleVertices(const ooctools::Triangle& _tri, fs::ofstream& _of);
		void writeTriangleVertices(const ooctools::Triangle& _tri, fs::fstream& _of);
		void writeTriangleNormals(const ooctools::Triangle& _tri, fs::ofstream& _of);
		void writeTriangleNormals(const ooctools::Triangle& _tri, fs::fstream& _of);
		void readTriangleVertices(ooctools::Triangle& _tri, fs::ifstream& _if);
		void readTriangleVertices(ooctools::Triangle& _tri, fs::fstream& _if);
		void readTriangleNormals(ooctools::Triangle& _tri, fs::ifstream& _if);
		void readTriangleNormals(ooctools::Triangle& _tri, fs::fstream& _if);
		void writeLooseOctreeNodeSkeleton(LooseOctree* _node, fs::ofstream& _of);
		LooseOctree* readLooseOctreeNodeSkeleton(fs::ifstream& _if);
		long double readLongDouble(fs::ifstream& _if);
		int64_t readLongLongInt(fs::ifstream& _if);
		void writeLongDouble(long double _ld, fs::ofstream& _of);
		void writeLongLongInt(int64_t _li, fs::ofstream& _of);

		double readDouble(fs::ifstream& _if);
		void writeDouble(double _d, fs::ofstream& _of);
		uint64_t readULongLongInt(fs::ifstream& _if);
		void writeULongLongInt(uint64_t _li, fs::ofstream& _of);

		/**
		 * @brief Tests whether the given directory exists and sets it if not.
		 * It does so recursively. This ensures the path will exist after the last
		 * recursion.
		 * @param _path The path to set.
		 */
		bool recursiveTestAndSet(fs::path _path);



};

}
#endif /* FILEIO_H_ */
