/*
 * RawModelWriter.cpp
 *
 *  Created on: 24.02.2009
 *      Author: ava
 */

#include "RawModelWriter.h"

#include <iostream>
#include <map>
#include <string>

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>

#include "FileHeader.h"
#include "MetaGroup.h"
#include "OOCTools.h"

#include "../../OOCTools/include/declarations.h"

using namespace std;
using namespace ooctools;
namespace fs = boost::filesystem;

namespace oocformats {

RawModelWriter::RawModelWriter() : constructorCalled(false)
{
	constructorCalled = true;
	// TODO Auto-generated constructor stub

}

RawModelWriter::~RawModelWriter()
{
	// TODO Auto-generated destructor stub
}

void
RawModelWriter::testWrite(MetaGroup* _grp)
{

	char* memblock;
	fs::path my_path("raw_objs/file.txt");
	fs::ofstream fb;
	fb.open(my_path, ios::binary | ios::out);
	fs::ofstream::pos_type size = 4 * sizeof(float);
	memblock = new char[size];
	fb.seekp(0, ios::beg);
	writeHeader(_grp, fb);
	//fb.write((char*)_data, size),
	fb.close();
	delete[] memblock;
}

void
RawModelWriter::testRead()
{
	void* memblock;
	fs::path my_path("raw_objs/file.txt");
	fs::ifstream fb;
	fb.open(my_path, ios::binary | ios::in);
	fs::ifstream::pos_type size = fb.tellg();
	memblock = new char[4 * sizeof(float)];
	fb.seekg(0, ios::beg);
	readHeader(fb);
	//fb.read((char*)memblock, 4*sizeof(float));
	//float ftest = readFloat(fb);
	//cout << "Size: " << size<< ((float*)memblock)[0]<< ", " << ((float*)memblock)[1] << ", " << ((float*)memblock)[2] << ", " << ((float*)memblock)[3] << ", " << ftest << endl;
	fb.close();
	delete[] (char*)memblock;
}

void
RawModelWriter::testWriteArrayf(float* farray, int entries)
{
	void* memblock;
	fs::path my_path("raw_objs/farray.txt");
	fs::ofstream fb;
	fb.open(my_path, ios::binary | ios::out);
	fs::ofstream::pos_type size = fb.tellp();

	memblock = new char[4 * sizeof(float)];
	fb.seekp(0, ios::beg);
	fb.write((char*) farray, entries * sizeof(float));
	cout << entries << " floats written:" << endl;
	for (int i = 0; i < entries; ++i) {
		cout << farray[i] << ", ";
		if (i % 5 == 0)
			cout << endl;
	}
	cout << "---------------------------------------" << endl;
	fb.close();
	delete[] (char*)memblock;
}

void
RawModelWriter::testReadArrayf(int entries)
{
	void* memblock;
	fs::path my_path("raw_objs/farray.txt");
	fs::ifstream fb;
	fb.open(my_path, ios::binary | ios::in);
	fs::ifstream::pos_type size = fb.tellg();
	memblock = new char[(entries - 2) * sizeof(float)];
	fb.seekg(0 + sizeof(float) * 2, ios::beg);
	fb.read((char*) memblock, (entries - 2) * sizeof(float));
	cout << entries << " floats read:" << endl;
	for (int i = 0; i < (entries - 2); ++i) {
		cout << ((float*) memblock)[i] << ", ";
		if (i % 5 == 0)
			cout << endl;
	}
	cout << "---------------------------------------" << endl;
	fb.close();
	delete[] (char*)memblock;
}
char*
RawModelWriter::readFileBytes(const char* name)
{
	fs::ifstream fl(name);
	fl.seekg(0, ios::end);
	size_t len = fl.tellg();
	char *ret = new char[len];
	fl.seekg(0, ios::beg);
	fl.read(ret, len);
	return ret;
}

bool
RawModelWriter::find_file(const fs::path& dir_path,
		const string& file_name, fs::path& path_found)
{
	if (!exists(dir_path))
		return false;
	fs::directory_iterator end_itr; // default construction yields past-the-end
	for (fs::directory_iterator itr(dir_path); itr != end_itr; ++itr) {
		if (fs::is_directory(itr->status())) {
			if (find_file(itr->path(), file_name, path_found))
				return true;
		}
		else if (itr->leaf() == file_name) // see below
		{
			path_found = itr->path();
			return true;
		}
	}
	return false;
}

void
RawModelWriter::writeV3f(const V3f& _v, fs::ofstream& _of)
{
	_of.write((char*) _v.x, sizeof(float));
	_of.write((char*) _v.y, sizeof(float));
	_of.write((char*) _v.z, sizeof(float));
}

V3f
RawModelWriter::readV3f(fs::ifstream& _if)
{
	V3f v;
	_if.read((char*) v.x, sizeof(float));
	_if.read((char*) v.y, sizeof(float));
	_if.read((char*) v.z, sizeof(float));
	return v;
}

void
RawModelWriter::writeV4f(const V4f& _v, fs::ofstream& _of)
{
	_of.write((char*) _v.x, sizeof(float));
	_of.write((char*) _v.y, sizeof(float));
	_of.write((char*) _v.z, sizeof(float));
	_of.write((char*) _v.w, sizeof(float));
}

V4f
RawModelWriter::readV4f(fs::ifstream& _if)
{
	V4f v;
	_if.read((char*) v.x, sizeof(float));
	_if.read((char*) v.y, sizeof(float));
	_if.read((char*) v.z, sizeof(float));
	_if.read((char*) v.w, sizeof(float));
	return v;
}

void
RawModelWriter::writeV3ub(V3ub* _v, fs::ofstream& _of)
{
	_of.write((char*) _v->x, sizeof(unsigned char));
	_of.write((char*) _v->y, sizeof(unsigned char));
	_of.write((char*) _v->z, sizeof(unsigned char));
}

V3ub*
RawModelWriter::readV3ub(fs::ifstream& _if)
{
	V3ub* v = new V3ub();
	*(v->x) = 125;
	_if.read((char*) v->x, sizeof(unsigned char));
	_if.read((char*) v->y, sizeof(unsigned char));
	_if.read((char*) v->z, sizeof(unsigned char));
	return v;
}

void
RawModelWriter::writeV3b(V3b* _v, fs::ofstream& _of)
{
	_of.write((char*) _v->x, sizeof(char));
	_of.write((char*) _v->y, sizeof(char));
	_of.write((char*) _v->z, sizeof(char));
}

void
RawModelWriter::writeBB(BoundingBox* _bb, fs::ofstream& _of)
{
	writeV3f(_bb->getMin(), _of);
	writeV3f(_bb->getMax(), _of);
}

void
RawModelWriter::writeVArrayf(VertexArray<float>* _va, fs::ofstream& _of)
{
	_of.write((char*) _va->mData, _va->size * _va->nComponents * sizeof(float));
}

void
RawModelWriter::writeVArrayb(VertexArray<char>* _va, fs::ofstream& _of)
{
	_of.write((char*) _va->mData, _va->size * _va->nComponents * sizeof(char));
}

void
RawModelWriter::writeInt(int _i, fs::ofstream& _of)
{
	_of.write((char*) &_i, sizeof(int));
}

void
RawModelWriter::writeUInt(unsigned int _i, fs::ofstream& _of)
{
	_of.write((char*) &_i, sizeof(unsigned int));
}

void
RawModelWriter::writeFloat(float _f, fs::ofstream& _of)
{
	_of.write((char*) &_f, sizeof(float));
}

char
RawModelWriter::readByte(fs::ifstream& _if)
{
	char b;
	_if.read((char*) &b, sizeof(char));
	return b;
}

unsigned char
RawModelWriter::readUByte(fs::ifstream& _if)
{
	unsigned char b;
	_if.read((char*) &b, sizeof(unsigned char));
	return b;
}

int
RawModelWriter::readInt(fs::ifstream& _if)
{
	int i;
	_if.read((char*) &i, sizeof(int));
	return i;
}

unsigned int
RawModelWriter::readUInt(fs::ifstream& _if)
{
	unsigned int i;
	_if.read((char*) &i, sizeof(unsigned int));
	return i;
}

float
RawModelWriter::readFloat(fs::ifstream& _if)
{
	float f;
	_if.read((char*) &f, sizeof(float));
	return f;
}

void
RawModelWriter::writeByte(char _b, fs::ofstream& _of)
{
	_of.write((char*) &_b, sizeof(char));
}

void
RawModelWriter::writeUByte(unsigned char _ub, fs::ofstream& _of)
{
	_of.write((char*) &_ub, sizeof(unsigned char));
}

void
RawModelWriter::writeHeader(MetaGroup* _grp, fs::ofstream& _of)
{
	// writing BoundingBox
	writeBB(_grp->bb, _of);

	writeUInt(_grp->nFaces, _of);

	// writing number of vertices to be extracted from this model group
	writeInt(_grp->nVertices, _of);
	// writing number of normals to
	writeInt(_grp->nNormals, _of);
	// TODO color support einfügen - ueber shader oder glcolor?
	//	writeV3ub(_grp->color, _of);
	writeUByte(255 * _grp->getMat().kdR, _of);
	writeUByte(255 * _grp->getMat().kdG, _of);
	writeUByte(255 * _grp->getMat().kdB, _of);
}

FileHeader
RawModelWriter::readHeader(fs::ifstream& _if)
{
	FileHeader fh;
	fh.bb = readBB(_if);
	fh.nFaces = readUInt(_if);
	fh.nVertices = readInt(_if);
	fh.nNormals = readInt(_if);
	fh.color = readV3ub(_if);
	return fh;
}

BoundingBox* RawModelWriter::readBB(fs::ifstream& _if)
{
	return new BoundingBox(readV3f(_if), readV3f(_if));
}

void
RawModelWriter::writeModel(Model* _model, fs::path _p)
{
	// file init stuff

	fs::ofstream of;
	map<string, MetaGroup*>::iterator it;

	testAndSetDir(_p);

	for (it = _model->getGrpStart(); it != _model->getGrpEnd(); ++it) {
		// notice: in phase1 we have seperate files for each group and each vertex array and normal array within each group
		// for reasons of completeness the header is contained within each of these files, vertex AND normalarrays
		// --------------------
		// osg screws a big time with object- and group-names
		// for reasons which are way beyond me, they put the complete path of the _original_ obj-file
		// as prefix into the groupname. This totally blows my plan to have a groupname represent a
		// directory. We have to check for this initially and remove this schmock!
		// --------------------
		string groupName(unscrewOsgGroupName(it->first));

		testAndSetDir(fs::path(_p / groupName));
//		cout << "this was passed in..... " << _p << endl;
//		cout << "this should be created..... " << fs::path(_p / groupName) << endl;
//		cout << "this is the orig. group name..... " << fs::path(it->first) << endl;
		fs::path localPath(_p / groupName / "vArray.bin");
		of.open(localPath, ios::binary | ios::out);
		of.seekp(0, ios::beg);
		writeHeader(it->second, of);
//		cout << "number of faces in group " << it->first << ": " << it->second->nFaces << endl;
		VertexArray<float>* floatVA = _model->getVArrayPtr(it);
		writeVArrayf(floatVA, of);
		of.close();
		delete floatVA;
		floatVA = 0;

		localPath = (_p / groupName / "nArray.bin");
		of.open(localPath, ios::binary | ios::out);
		of.seekp(0, ios::beg);
		writeHeader(it->second, of);
		VertexArray<char>* charVA = _model->getNArrayPtr(it);
		writeVArrayb(charVA, of);
		delete charVA;
		charVA = 0;
		of.close();
	}
}

void
RawModelWriter::testAndSetDir(fs::path _p)
{
	if (!exists(_p)) {
		fs::create_directory(_p);
	}
}

//map<std::string, VBO*> *Phase1ModelWriter::readModel(fs::path _p){
//	map<std::string, VBO*> *vboMap = new map<std::string, VBO*>;
//	if ( !exists( _p ) ){
//		return 0;
//	}
//	fs::directory_iterator dir_iter(_p), dir_end;
//	for(;dir_iter != dir_end; ++dir_iter)
//	{
//		// iter contains an entry in the starting path _p - hopefully a directory
//		if ( is_directory(dir_iter->status()) ){
//			VBO *vbo = new VBO();
//			if( !exists( fs::path(dir_iter->path() / "vArray.bin") )){
//				cout << "gibt keinen vertexarray" << endl;
//				return 0;
//			}
//			else{
//				fs::ifstream fb;
//				fb.open( fs::path(dir_iter->path() / "vArray.bin"), ios::binary|ios::in);
//				fb.seekg(0, ios::beg);
//				FileHeader fh = readHeader(fb);
//				fb.seekg(getHeaderSize(), ios::beg);
//				float *fa = readFloatArray(fb, 3*fh.vertexCount);
//				VertexArray *va = new VertexArray(3,fh.vertexCount, fa);
//				va->setBB(fh.bb);
//				vbo->setVData(va);
//				fb.close();
//			}
//
//			if( !exists( fs::path(dir_iter->path() / "nArray.bin") )){
//				cout << "gibt keinen Normalarray" << endl;
//				return 0;
//			}
//			else{
//				fs::ifstream fb;
//				fb.open( fs::path(dir_iter->path() / "nArray.bin"), ios::binary|ios::in);
//				FileHeader fh = readHeader(fb);
//				fb.seekg(getHeaderSize(), ios::beg);
//				char *ba = readByteArray(fb, 4*fh.normalCount);
//				VertexArray *va = new VertexArray(4,fh.normalCount, ba);
//				vbo->setNData(va);
//			}
//
//			vboMap->insert(make_pair( dir_iter->leaf(), vbo ));
//		}
//
//		if( !exists( dir_iter->path() )){
//			return 0;
//		}
//	}
//
//	return vboMap;
//}

VboManager*
RawModelWriter::readModel(fs::path _p, const ColorTable& _ct)
{
	VboManager* vboMan = VboManager::getInstancePtr();
	if (!fs::exists(_p)) {
		cerr << "The path " << _p << " does not exist!" << endl;
		return 0;
	}
	vboMan->setColorTable(_ct);
	fs::directory_iterator dir_iter(_p), dir_end;
	for (; dir_iter != dir_end; ++dir_iter) {
		// iter contains an entry in the starting path _p - hopefully a directory
		if (is_directory(dir_iter->status())) {
			Vbo* vbo = new Vbo();
			if (!exists(fs::path(dir_iter->path() / "vArray.bin"))) {
				cout << "gibt keinen vertexarray" << endl;
				return 0;
			}
			else {
				fs::ifstream fb;
				fb.open(fs::path(dir_iter->path() / "vArray.bin"), ios::binary
						| ios::in);
				fb.seekg(0, ios::beg);
				FileHeader fh = readHeader(fb);
				fb.seekg(getHeaderSize(), ios::beg);
				float* fa = readFloatArray(fb, 4 * fh.nVertices);
				VertexArray<float>* va = new VertexArray<float>(4, fh.nVertices, fa);
				va->setBB(*fh.bb);
				vbo->setVData(va);
//				vbo->setColor(fh.color);
				fb.close();
			}

			if (!exists(fs::path(dir_iter->path() / "nArray.bin"))) {
				cout << "gibt keinen Normalarray" << endl;
				return 0;
			}
			else {
				fs::ifstream fb;
				fb.open(fs::path(dir_iter->path() / "nArray.bin"), ios::binary
						| ios::in);
				FileHeader fh = readHeader(fb);
				fb.seekg(getHeaderSize(), ios::beg);
				char* ba = readByteArray(fb, 4 * fh.nNormals);
				VertexArray<char>* va = new VertexArray<char>(4, fh.nNormals, ba);
				vbo->setNData(va);
			}
			string vboName(dir_iter->path().parent_path().filename());
			vboName.append("-");
			vboName.append(dir_iter->leaf());
			vboMan->addVbo(vboName, vbo);
//			cout << "parentname: " << dir_iter->path().parent_path().filename() << endl;
//			cout << "groupname: " << dir_iter->leaf() << endl;
		}

		if (!fs::exists(dir_iter->path())) {
			return 0;
		}
	}

	return vboMan;
}

VboManager*
RawModelWriter::readModel(fs::path _p)
{
	ColorTable ct(_p / "colortable.bin");
	return readModel(_p, ct);
}

float*
RawModelWriter::readFloatArray(fs::ifstream& _if, int count)
{
	//cout << "reading " << count*sizeof(float) << " bytes as float" << endl;
	char* data = new char[count * sizeof(float)];
	_if.read((char*) data, count * sizeof(float));
	return (float*) data;
}

char*
RawModelWriter::readByteArray(fs::ifstream &_if, int count)
{
	char* data = new char[count * sizeof(char)];
	_if.read((char*) data, count * sizeof(char));
	return (char*) data;
}

string
RawModelWriter::unscrewOsgGroupName(const string& _gName)
{
	if (_gName.substr(0,8) == "/media/E") {
		string::size_type loc = _gName.find(".obj");
		if (loc != string::npos){
			string temp(_gName.substr(loc+5));
//			cout << "das haben wir: " << temp << endl;
			removeSpecialCharsFromName(temp);
			return temp;
		}
	}
	string temp(_gName);
	removeSpecialCharsFromName(temp);
	return temp;
}

void
RawModelWriter::removeSpecialCharsFromName(std::string& _origin)
{
	unsigned int nChanges = 0;
	std::string sChars("!*~$|?%§;{}#<>:\" /\\");
	std::string::size_type found = _origin.find_first_of(sChars);
	while(found != std::string::npos){
		_origin[found] = '_';
		++nChanges;
		found = _origin.find_first_of(sChars, found+1);
	}
	if (nChanges>0) {
		_origin.append("_");
		_origin.append(boost::lexical_cast<std::string>(nChanges));
	}
}

} // oocformats
