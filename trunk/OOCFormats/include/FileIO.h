/**
 * @file	FileIO.h
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 16.04.2009
 * @brief   Class declaration of FileIO
 */

#ifndef FILEIO_H_
#define FILEIO_H_

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
		static void writeV3f(const ooctools::V3f& _v, fs::ofstream& _of);

		static ooctools::V3f readV3f(fs::ifstream& _if);
		static void writeV4f(const ooctools::V4f& _v, fs::ofstream& _of);
		static ooctools::V4f readV4f(fs::ifstream& _if);
		static void writeV3ub(ooctools::V3ub* _v, fs::ofstream& _of);
		static ooctools::V3ub* readV3ub(fs::ifstream& _if);
		static void writeV3b(ooctools::V3b* _v, fs::ofstream& _of);
		static void writeBB(ooctools::BoundingBox* _bb, fs::ofstream& _of);
		static void writeVArrayf(const ooctools::VertexArray<float>* _va, fs::ofstream& _of);
		static void writeVArrayb(const ooctools::VertexArray<char>* _va, fs::ofstream& _of);
		static void writeInt(int _i, fs::ofstream& _of);
		static void writeUInt(unsigned int _i, fs::ofstream& _of);
		static void writeFloat(float _f, fs::ofstream& _of);
		static char readByte(fs::ifstream& _if);
		static unsigned char readUByte(fs::ifstream& _if);
		static int readInt(fs::ifstream& _if);
		static unsigned int readUInt(fs::ifstream& _if);
		static float readFloat(fs::ifstream& _if);
		static void writeByte(char _b, fs::ofstream& _of);
		static void writeUByte(unsigned char _ub, fs::ofstream& _of);
		static float* readFloatArray(fs::ifstream& _if, int count);
		static char* readByteArray(fs::ifstream &_if, int count);
		static ooctools::BoundingBox* readBB(fs::ifstream& _if);
		static FileHeader readHeader(fs::ifstream& _if);

		/**
		 * @brief Tests whether the given directory exists and sets it if not.
		 * It does so recursively. This ensures the path will exist after the last
		 * recursion.
		 * @param _path The path to set.
		 */
		static bool recursiveTestAndSet(fs::path _path);



};

}
#endif /* FILEIO_H_ */
