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

#include "FileHeader.h"
#include "V3f.h"
#include "V4f.h"
#include "V3ub.h"
#include "V3b.h"
#include "BoundingBox.h"
#include "Triangle.h"
#include "VertexArray.h"
#include "Vbo.h"
#include "declarations.h"
#include "MetaGroup.h"
#include "LooseOctree.h"
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
FileIO::readV3f(V3f& v, fs::ifstream& _if)
{
	_if.read((char*) v.x, sizeof(float));
	_if.read((char*) v.y, sizeof(float));
	_if.read((char*) v.z, sizeof(float));
}

void
FileIO::readV3f(V3f& v, fs::fstream& _if)
{
	_if.read((char*) v.x, sizeof(float));
	_if.read((char*) v.y, sizeof(float));
	_if.read((char*) v.z, sizeof(float));
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

V3ub*
FileIO::readV3ub(fs::fstream& _if)
{
	V3ub* v = new V3ub();
	*(v->x) = 125;
	_if.read((char*) v->x, sizeof(unsigned char));
	_if.read((char*) v->y, sizeof(unsigned char));
	_if.read((char*) v->z, sizeof(unsigned char));
	return v;
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
FileIO::writeV3f(const V3f& _v, fs::ofstream& _of)
{
	_of.write((char*) _v.x, sizeof(float));
	_of.write((char*) _v.y, sizeof(float));
	_of.write((char*) _v.z, sizeof(float));
}

void
FileIO::writeV3f(const V3f& _v, fs::fstream& _of)
{
	_of.write((char*) _v.x, sizeof(float));
	_of.write((char*) _v.y, sizeof(float));
	_of.write((char*) _v.z, sizeof(float));
}

void
FileIO::writeV3ub(V3ub* _v, fs::ofstream& _of)
{
	_of.write((char*) _v->x, sizeof(unsigned char));
	_of.write((char*) _v->y, sizeof(unsigned char));
	_of.write((char*) _v->z, sizeof(unsigned char));
}

void
FileIO::writeV3ub(V3ub* _v, fs::fstream& _of)
{
	_of.write((char*) _v->x, sizeof(unsigned char));
	_of.write((char*) _v->y, sizeof(unsigned char));
	_of.write((char*) _v->z, sizeof(unsigned char));
}

void
FileIO::writeV4f(const V4f& _v, fs::ofstream& _of)
{
	_of.write((char*) _v.x, sizeof(float));
	_of.write((char*) _v.y, sizeof(float));
	_of.write((char*) _v.z, sizeof(float));
	_of.write((char*) _v.w, sizeof(float));
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
FileIO::writeBB(BoundingBox* _bb, fs::fstream& _of)
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
FileIO::writeInt(int _i, fs::fstream& _of)
{
	_of.write((char*) &_i, sizeof(int));
}

void
FileIO::writeUInt(unsigned int _i, fs::ofstream& _of)
{
	_of.write((char*) &_i, sizeof(unsigned int));
}

void
FileIO::writeUInt(unsigned int _i, fs::fstream& _of)
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
	int i = 0;
	_if.read((char*) &i, sizeof(int));
	return i;
}

int
FileIO::readInt(fs::fstream& _if)
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

unsigned int
FileIO::readUInt(fs::fstream& _if)
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
FileIO::readFloatArray(fs::ifstream& _if, unsigned count)
{
	//cout << "reading " << count*sizeof(float) << " bytes as float" << endl;
	char* data = new char[count * sizeof(float)];
	_if.read((char*) data, count * sizeof(float));
	return (float*) data;
}

char*
FileIO::readByteArray(fs::ifstream &_if, unsigned count)
{
	char* data = new char[count * sizeof(char)];
	_if.read((char*) data, count * sizeof(char));
	return (char*) data;
}

BoundingBox*
FileIO::readBB(fs::ifstream& _if)
{
	V3f* min = new V3f(0.0);
	readV3f(*min, _if);
	V3f* max = new V3f(0.0);
	readV3f(*max, _if);
	BoundingBox* bb = new BoundingBox(*min, *max);
	delete min;
	delete max;
	return bb;
}

BoundingBox*
FileIO::readBB(fs::fstream& _if)
{
	V3f* min = new V3f(0.0);
	readV3f(*min, _if);
	V3f* max = new V3f(0.0);
	readV3f(*max, _if);
	BoundingBox* bb = new BoundingBox(*min, *max);
	delete min;
	delete max;
	return bb;
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

void
FileIO::readHeader(FileHeader& fh, fs::ifstream& _if)
{
	fh.bb = FileIO::readBB(_if);
	fh.nFaces = FileIO::readUInt(_if);
	fh.nVertices = FileIO::readInt(_if);
	fh.nNormals = FileIO::readInt(_if);
	fh.color = FileIO::readV3ub(_if);
}

void
FileIO::readHeader(FileHeader& fh, fs::fstream& _if)
{
	fh.bb = FileIO::readBB(_if);
	fh.nFaces = FileIO::readUInt(_if);
	fh.nVertices = FileIO::readInt(_if);
	fh.nNormals = FileIO::readInt(_if);
	fh.color = FileIO::readV3ub(_if);
}


void
FileIO::writeHeader(MetaGroup* _grp, fs::ofstream& _of)
{
	// writing BoundingBox
	FileIO::writeBB(_grp->bb, _of);

	FileIO::writeUInt(_grp->nFaces, _of);

	// writing number of vertices to be extracted from this model group
	FileIO::writeInt(_grp->nVertices, _of);
	// writing number of normals to
	FileIO::writeInt(_grp->nNormals, _of);
	// TODO color support einfügen - ueber shader oder glcolor?
	//	writeV3ub(_grp->color, _of);
	FileIO::writeUByte(255 * _grp->getMat().kdR, _of);
	FileIO::writeUByte(255 * _grp->getMat().kdG, _of);
	FileIO::writeUByte(255 * _grp->getMat().kdB, _of);
}

void
FileIO::writeHeader(FileHeader& _header, fs::ofstream& _of)
{
	// writing BoundingBox
	FileIO::writeBB(_header.bb, _of);

	FileIO::writeUInt(_header.nFaces, _of);

	// writing number of vertices to be extracted from this model group
	FileIO::writeInt(_header.nVertices, _of);
	// writing number of normals to
	FileIO::writeInt(_header.nNormals, _of);
	FileIO::writeV3ub(_header.color,_of);
}

void
FileIO::writeHeader(FileHeader& _header, fs::fstream& _of)
{
	// writing BoundingBox
	FileIO::writeBB(_header.bb, _of);

	FileIO::writeUInt(_header.nFaces, _of);

	// writing number of vertices to be extracted from this model group
	FileIO::writeInt(_header.nVertices, _of);
	// writing number of normals to
	FileIO::writeInt(_header.nNormals, _of);
	FileIO::writeV3ub(_header.color,_of);
}

void
FileIO::writeTriList(const std::list<Triangle>& _triList, fs::ofstream& _vFile, fs::ofstream& _nFile)
{
	list<Triangle>::const_iterator it = _triList.begin();
	for(; it != _triList.end(); ++it){
		writeTriangleVertices(*it, _vFile);
		writeTriangleNormals(*it, _nFile);
	}
}

void
FileIO::writeTriangleVertices(const Triangle& _tri, fs::ofstream& _of)
{
	_of.write((char*) _tri.getVertices(), 3 * 4 * sizeof(float));
}

void
FileIO::writeTriangleVertices(const Triangle& _tri, fs::fstream& _of)
{
	_of.write((char*) _tri.getVertices(), 3 * 4 * sizeof(float));
}

void
FileIO::writeTriangleNormals(const Triangle& _tri, fs::ofstream& _of)
{
	_of.write((char*) _tri.getNormals(), 3 * 4 * sizeof(char));
}

void
FileIO::writeTriangleNormals(const Triangle& _tri, fs::fstream& _of)
{
	_of.write((char*) _tri.getNormals(), 3 * 4 * sizeof(char));
}

void
FileIO::writeLooseOctreeNodeSkeleton(LooseOctree* _node, fs::ofstream& _of)
{
	void* nodeSkel = new char[LooseOctree::getSkeletonSize()];
	_node->getNodeSkeleton(nodeSkel);
	// id, 8 ids, 6 floats, long double area, uint tricount
	_of.write((char*)nodeSkel, LooseOctree::getSkeletonSize());
	_of.flush();
	delete[] (char*)nodeSkel;
	//TODO
}

LooseOctree*
FileIO::readLooseOctreeNodeSkeleton(fs::ifstream& _if)
{
	char nodeSkel[LooseOctree::getSkeletonSize()];
	// id, 8 ids, 6 floats, long double area, uint tricount
	_if.read((char*)nodeSkel, LooseOctree::getSkeletonSize());
	//TODO
	LooseOctree* node = new LooseOctree(nodeSkel);

	return node;
}

LooseRenderOctree*
FileIO::readLooseRenderOctreeNodeSkeleton(fs::ifstream& _if)
{
	char nodeSkel[LooseRenderOctree::getSkeletonSize()];
	// id, 8 ids, 6 floats, long double area, uint tricount
	_if.read((char*)nodeSkel, LooseRenderOctree::getSkeletonSize());
	//TODO
	LooseRenderOctree* node = new LooseRenderOctree(nodeSkel);

	return node;
}

long double
FileIO::readLongDouble(fs::ifstream& _if)
{
	long double ld;
	_if.read((char*) &ld, sizeof(long double));
	return ld;
}

int64_t
FileIO::readLongLongInt(fs::ifstream& _if)
{
	int64_t li;
	_if.read((char*) &li, sizeof(int64_t));
	return li;
}

void
FileIO::writeLongDouble(long double _ld, fs::ofstream& _of)
{
	_of.write((char*) &_ld, sizeof(long double));
}

void
FileIO::writeLongLongInt(int64_t _li, fs::ofstream& _of)
{
	_of.write((char*) &_li, sizeof(int64_t));
}

double
FileIO::readDouble(fs::ifstream& _if)
{
	double d;
	_if.read((char*) &d, sizeof(double));
	return d;
}

void
FileIO::writeDouble(double _d, fs::ofstream& _of)
{
	_of.write((char*) &_d, sizeof(double));
}

uint64_t
FileIO::readULongLongInt(fs::ifstream& _if)
{
	uint64_t ulli;
	_if.read((char*) &ulli, sizeof(uint64_t));
	return ulli;
}

void
FileIO::writeULongLongInt(uint64_t _li, fs::ofstream& _of)
{
	_of.write((char*) &_li, sizeof(uint64_t));
}

void
FileIO::readTriangleVertices(ooctools::Triangle& _tri, fs::ifstream& _if)
{
	float* vertices = new float[3 * 4];
	_if.read((char*) vertices, 3 * 4 * sizeof(float));
	_tri.setVertices(vertices);
	delete[] vertices;
}

void
FileIO::readTriangleVertices(ooctools::Triangle& _tri, fs::fstream& _if)
{
	float* vertices = new float[3 * 4];
	_if.read((char*) vertices, 3 * 4 * sizeof(float));
	_tri.setVertices(vertices);
	delete[] vertices;
}

void
FileIO::readTriangleNormals(ooctools::Triangle& _tri, fs::ifstream& _if)
{
	char* normals = new char[3 * 4];
	_if.read((char*) normals, 3 * 4 * sizeof(char));
	_tri.setNormals(normals);
	delete[] normals;
}

void
FileIO::readTriangleNormals(ooctools::Triangle& _tri, fs::fstream& _if)
{
	char* normals = new char[3 * 4];
	_if.read((char*) normals, 3 * 4 * sizeof(char));
	_tri.setNormals(normals);
	delete[] normals;
}

} //oocformats
