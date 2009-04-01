/*
 * RawModelWriter.h
 *
 *  Created on: 24.02.2009
 *      Author: ava
 */

#ifndef RAWMODELWRITER_H_
#define RAWMODELWRITER_H_

#include "boost/filesystem.hpp"
#include <boost/filesystem/fstream.hpp>
#include <string>
#include <map>

#include "declarations.h"
#include "../../OOCTools/include/declarations.h"

namespace fs = boost::filesystem;

namespace oocformats {

class RawModelWriter {
public:
	RawModelWriter();
	virtual ~RawModelWriter();
	void testWrite(ooctools::MetaGroup *_grp);
	bool find_file(const fs::path & dir_path, const std::string & file_name, fs::path & path_found );
	char* readFileBytes(const char *name);
	void testRead();
	void writeV3f(const ooctools::V3f &_v, fs::ofstream &_of);
	void writeV3b(ooctools::V3b *_v, fs::ofstream &_of);
	void writeV3ub(ooctools::V3ub *_v, fs::ofstream &_of);
	void writeV4f(const ooctools::V4f &_v, fs::ofstream &_of);
	void writeBB(ooctools::BoundingBox *_bb, fs::ofstream &_of);
	void writeVArrayf(ooctools::VertexArray<float>* _va, fs::ofstream &_of);
	void writeVArrayb(ooctools::VertexArray<char>* _va, fs::ofstream &_of);
	void writeInt(int _i, fs::ofstream &_of);
	void writeFloat(float _f, fs::ofstream &_of);
	void writeByte(char _c, fs::ofstream &_of);
	void writeUByte(unsigned char _ub, fs::ofstream &_of);
	int getHeaderSize(){return 35;};

	/*
	 * Header-Format: 3xfloat32[minBB], 3xfloat32[maxBB], int32[vertCount], int32[normCount], 3xuchar[color]
	 * Headersize: 35 byte
	 */
	void writeHeader(ooctools::MetaGroup *_grp, fs::ofstream &_of);
	FileHeader readHeader(fs::ifstream &_if);
	ooctools::BoundingBox *readBB(fs::ifstream &_if);
	ooctools::V3f readV3f(fs::ifstream &_if);
	ooctools::V3ub *readV3ub(fs::ifstream &_if);
	ooctools::V4f readV4f(fs::ifstream &_if);
	char readByte(fs::ifstream &_if);
	int readInt(fs::ifstream &_if);
	unsigned char readUByte(fs::ifstream &_if);
	float readFloat(fs::ifstream &_if);
	float *readFloatArray(fs::ifstream &_if, int count);
	char *readByteArray(fs::ifstream &_if, int count);
	/*
	 * @params _model a pointer to the model, which will be saved as binary file
	 * @params _p a path (not a file) wher to save the model-files
	 */
	void writeModel(ooctools::Model *_model, fs::path _p);
	//std::map<std::string, OOCTools::VBO*> *readModel(fs::path _p);
	ooctools::VboManager *readModel(fs::path _p);
	void testWriteArrayf(float *farray, int entries);
	void testReadArrayf(int entries);

private:
	void testAndSetDir(fs::path _p);
	std::string unscrewOsgGroupName(const std::string& _gName);

};

} // oocformats

#endif /* RAWMODELWRITER_H_ */
