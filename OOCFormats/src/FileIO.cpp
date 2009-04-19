/**
 * @file	FileIO.cpp
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 16.04.2009
 *
 */

#include "FileIO.h"

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "V3f.h"
#include "V4f.h"
#include "V3ub.h"
#include "V3b.h"
#include "BoundingBox.h"
#include "VertexArray.h"
#include "Vbo.h"
#include "declarations.h"
#include "../../OOCTools/include/declarations.h"

using namespace ooctools;
using namespace std;
namespace oocformats {

FileIO::FileIO()
{
	// TODO Auto-generated constructor stub

}

FileIO::~FileIO()
{
	// TODO Auto-generated destructor stub
}

void
FileIO::writeV3f(const V3f& _v, fs::ofstream& _of)
{
	_of.write((char*) _v.x, sizeof(float));
	_of.write((char*) _v.y, sizeof(float));
	_of.write((char*) _v.z, sizeof(float));
}

V3f
FileIO::readV3f(fs::ifstream& _if)
{
	V3f v;
	_if.read((char*) v.x, sizeof(float));
	_if.read((char*) v.y, sizeof(float));
	_if.read((char*) v.z, sizeof(float));
	return v;
}

void
FileIO::writeV4f(const V4f& _v, fs::ofstream& _of)
{
	_of.write((char*) _v.x, sizeof(float));
	_of.write((char*) _v.y, sizeof(float));
	_of.write((char*) _v.z, sizeof(float));
	_of.write((char*) _v.w, sizeof(float));
}

V4f
FileIO::readV4f(fs::ifstream& _if)
{
	V4f v;
	_if.read((char*) v.x, sizeof(float));
	_if.read((char*) v.y, sizeof(float));
	_if.read((char*) v.z, sizeof(float));
	_if.read((char*) v.w, sizeof(float));
	return v;
}

void
FileIO::writeV3ub(V3ub* _v, fs::ofstream& _of)
{
	_of.write((char*) _v->x, sizeof(unsigned char));
	_of.write((char*) _v->y, sizeof(unsigned char));
	_of.write((char*) _v->z, sizeof(unsigned char));
}

V3ub*
FileIO::readV3ub(fs::ifstream& _if)
{
	V3ub* v = new V3ub();
	*(v->x) = 125;
	_if.read((char*) v->x, sizeof(unsigned char));
	_if.read((char*) v->y, sizeof(unsigned char));
	_if.read((char*) v->z, sizeof(unsigned char));
	return v;
}

void
FileIO::writeV3b(V3b* _v, fs::ofstream& _of)
{
	_of.write((char*) _v->x, sizeof(char));
	_of.write((char*) _v->y, sizeof(char));
	_of.write((char*) _v->z, sizeof(char));
}

void
FileIO::writeBB(BoundingBox* _bb, fs::ofstream& _of)
{
	writeV3f(_bb->getMin(), _of);
	writeV3f(_bb->getMax(), _of);
}

void
FileIO::writeVArrayf(const VertexArray<float>* _va, fs::ofstream& _of)
{
	_of.write((char*) _va->mData, _va->size * _va->nComponents * sizeof(float));
}

void
FileIO::writeVArrayb(const VertexArray<char>* _va, fs::ofstream& _of)
{
	_of.write((char*) _va->mData, _va->size * _va->nComponents * sizeof(char));
}

void
FileIO::writeInt(int _i, fs::ofstream& _of)
{
	_of.write((char*) &_i, sizeof(int));
}

void
FileIO::writeUInt(unsigned int _i, fs::ofstream& _of)
{
	_of.write((char*) &_i, sizeof(unsigned int));
}

void
FileIO::writeFloat(float _f, fs::ofstream& _of)
{
	_of.write((char*) &_f, sizeof(float));
}

char
FileIO::readByte(fs::ifstream& _if)
{
	char b;
	_if.read((char*) &b, sizeof(char));
	return b;
}

unsigned char
FileIO::readUByte(fs::ifstream& _if)
{
	unsigned char b;
	_if.read((char*) &b, sizeof(unsigned char));
	return b;
}

int
FileIO::readInt(fs::ifstream& _if)
{
	int i;
	_if.read((char*) &i, sizeof(int));
	return i;
}

unsigned int
FileIO::readUInt(fs::ifstream& _if)
{
	unsigned int i;
	_if.read((char*) &i, sizeof(unsigned int));
	return i;
}

float
FileIO::readFloat(fs::ifstream& _if)
{
	float f;
	_if.read((char*) &f, sizeof(float));
	return f;
}

void
FileIO::writeByte(char _b, fs::ofstream& _of)
{
	_of.write((char*) &_b, sizeof(char));
}

void
FileIO::writeUByte(unsigned char _ub, fs::ofstream& _of)
{
	_of.write((char*) &_ub, sizeof(unsigned char));
}

float*
FileIO::readFloatArray(fs::ifstream& _if, int count)
{
	//cout << "reading " << count*sizeof(float) << " bytes as float" << endl;
	char* data = new char[count * sizeof(float)];
	_if.read((char*) data, count * sizeof(float));
	return (float*) data;
}

char*
FileIO::readByteArray(fs::ifstream &_if, int count)
{
	char* data = new char[count * sizeof(char)];
	_if.read((char*) data, count * sizeof(char));
	return (char*) data;
}

BoundingBox* FileIO::readBB(fs::ifstream& _if)
{
	return new BoundingBox(readV3f(_if), readV3f(_if));
}

bool
FileIO::recursiveTestAndSet(fs::path _path)
{
//	cout << "path: " << _path << endl;
//	cout << "parent: " << _path.parent_path().parent_path().parent_path() << endl;
//	cout << "rootDir: " << _path.root_directory() << endl;
//	cout << "rootname: " << _path.root_name() << endl;
//	cout << "rootPath: " << _path.root_path() << endl;
//	return false;
	if(!fs::exists(_path)){
		FileIO::recursiveTestAndSet(_path.parent_path());
		fs::create_directory(_path);
	}
	return true;
}

} //oocformats
