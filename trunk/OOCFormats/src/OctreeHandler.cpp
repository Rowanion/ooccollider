/**
 * @file	OctreeHandler.cpp
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 23.03.2009
 * @brief 	OctreeHandler class definition.
 */

#include "OctreeHandler.h"

#include "boost/filesystem.hpp"
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>

#include "declarations.h"
#include "../../OOCTools/include/declarations.h"
#include "../../OOCFormats/include/declarations.h"
#include "Octree.h"
#include "OctreeNode.h"
#include "FileIO.h"
#include "VboManager.h"

using namespace std;
using namespace oocformats;
using namespace ooctools;
namespace fs = boost::filesystem;
namespace oocformats {

OctreeHandler::OctreeHandler() : mPriOctree(0)
{
	// TODO Auto-generated constructor stub

}

OctreeHandler::~OctreeHandler()
{
	// TODO check if we really have to delete the octree here....
	delete mPriOctree;
	mPriOctree = 0;
}

// recursive
void
OctreeHandler::parsePhase1(const fs::path& src, const fs::path& dst)
{
	// if is dir -> recurse
	// else if (is filename == "vArray.bin"){
	// 	read vArray AND nArray.bin via special moWri VBO-method
	//	insert vbo into octree
	// }
}

void
OctreeHandler::generateOctree(const fs::path& src, const fs::path& dst, const BoundingBox& _bb)
{
	if (mPriOctree == 0)
		mPriOctree = new Octree(_bb, dst.directory_string());
	// foreach
	fs::directory_iterator dir_iter(src), dir_end;
	for (; dir_iter != dir_end; ++dir_iter) {
		if (fs::is_directory(dir_iter->status())) {
			cout << "is dir? " << dir_iter->path()<< endl;
			fs::path p(dir_iter->path());
			generateOctree(p, dst, 0);
		}
		else if (dir_iter->path().filename() == "vArray.bin"){
			Vbo* v = RawModelWriter::readRawVbo(dir_iter->path().parent_path());
			mPriOctree->insertVbo(*v);
		}
	}

	parsePhase1(src, dst);
	//
}

void
OctreeHandler::writeOctreeVbo(const Vbo& _vbo, fs::path _path){
	cout << "writing vbo: " << _vbo.getVData()->size << ", " << _vbo.getVData()->mData[0] << endl;
	fs::ofstream of;
	string s(_path.string());
	of.open(fs::path(_path / "vArray.oct"), ios::binary | ios::out);
	of.seekp(0, ios::beg);
	FileIO::writeUInt(_vbo.getVData()->size, of);
	FileIO::writeUInt(_vbo.getVData()->nComponents, of);
	of.seekp(2*sizeof(unsigned int), ios::beg);
	FileIO::writeVArrayf(_vbo.getVData(), of);
	of.close();

	of.open(fs::path(_path / "nArray.oct"), ios::binary | ios::out);
	of.seekp(0, ios::beg);
	FileIO::writeUInt(_vbo.getNData()->size, of);
	FileIO::writeUInt(_vbo.getNData()->nComponents, of);
	of.seekp(2*sizeof(unsigned int), ios::beg);
	FileIO::writeVArrayb(_vbo.getNData(), of);
	of.close();
}

Vbo*
OctreeHandler::readOctreeVbo(fs::path _path){
	cout << "path:      " << _path << endl;
	if (!fs::exists(_path / "vArray.oct"))
		return 0;
	else {
		Vbo* vbo = new Vbo();
		fs::ifstream in;
		in.open(fs::path(_path / "vArray.oct"), ios::binary | ios::in);
		in.seekg(0, ios::beg);
		unsigned int size = FileIO::readUInt(in);
		unsigned int comp = FileIO::readUInt(in);
		in.seekg(2*sizeof(unsigned int), ios::beg);
		float* fData = FileIO::readFloatArray(in, size*comp);
		VertexArray<float>* va = new VertexArray<float>(comp, size, fData);
		va->calcBB();
		in.close();
		vbo->setVData(va, false);

		in.open(fs::path(_path / "nArray.oct"), ios::binary | ios::in);
		in.seekg(0, ios::beg);
		size = FileIO::readUInt(in);
		comp = FileIO::readUInt(in);
		in.seekg(2*sizeof(unsigned int), ios::beg);
		char* cData = FileIO::readByteArray(in, size*comp);
		VertexArray<char>* na = new VertexArray<char>(comp, size, cData);
		in.close();
		vbo->setNData(na, false);

		cout << "reading vbo: " << vbo->getVData()->size << ", " << vbo->getVData()->mData[0] << endl;

		return vbo;
	}
}

//TODO rewrite it, so that the tree is "complete"
void
OctreeHandler::readOctreeRecursive(fs::path _path, Octree* tree)
{

	fs::directory_iterator dir_iter(_path), dir_end;
	for (; dir_iter != dir_end; ++dir_iter) {
		if (fs::is_directory(dir_iter->status())){
			readOctreeRecursive(dir_iter->path(), tree);
		}
		else if (dir_iter->path().filename() == "vArray.oct"){
			Vbo* vbo = readOctreeVbo(dir_iter->path().parent_path());
			vbo->setOnline();
			VboManager::getSingleton()->addVbo(boost::lexical_cast<std::string>(VboManager::getSingleton()->getVboCount()), vbo);
		}
	}


}

} // oocformats
