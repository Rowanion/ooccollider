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
#include "FileIO.h"
#include "VboManager.h"
#include "LooseProcessingOctree.h"
#include "LooseOctree.h"
#include "StructDefs.h"
#include "FileHeader.h"
#include "Triangle.h"
#include "TriangleStream.h"
#include "Vbo.h"

using namespace std;
using namespace oocformats;
using namespace ooctools;
namespace fs = boost::filesystem;
namespace oocformats {

OctreeHandler::OctreeHandler() : mProcOctree(0), mFio(FileIO()),
		rmh(RawModelHandler())
{
	// TODO Auto-generated constructor stub
	codeMap.insert(make_pair(string("fne"),0));
	codeMap.insert(make_pair(string("fnw"),1));
	codeMap.insert(make_pair(string("fse"),2));
	codeMap.insert(make_pair(string("fsw"),3));
	codeMap.insert(make_pair(string("bne"),4));
	codeMap.insert(make_pair(string("bnw"),5));
	codeMap.insert(make_pair(string("bse"),6));
	codeMap.insert(make_pair(string("bsw"),7));

}

OctreeHandler::~OctreeHandler()
{
	// TODO check if we really have to delete the octree here....
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
OctreeHandler::writeOctreeVbo(const Vbo& _vbo, fs::path _path){
	cout << "writing vbo: " << _vbo.getVData()->size << ", " << _vbo.getVData()->mData[0] << endl;
	fs::ofstream of;
	string s(_path.string());
	of.open(fs::path(_path / "vArray.oct"), ios::binary | ios::out);
	of.seekp(0, ios::beg);
	mFio.writeUInt(_vbo.getVData()->size, of);
	mFio.writeUInt(_vbo.getVData()->nComponents, of);
	of.seekp(2*sizeof(unsigned int), ios::beg);
	mFio.writeVArrayf(_vbo.getVData(), of);
	of.close();

	of.open(fs::path(_path / "nArray.oct"), ios::binary | ios::out);
	of.seekp(0, ios::beg);
	mFio.writeUInt(_vbo.getNData()->size, of);
	mFio.writeUInt(_vbo.getNData()->nComponents, of);
	of.seekp(2*sizeof(unsigned int), ios::beg);
	mFio.writeVArrayb(_vbo.getNData(), of);
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
		unsigned int size = mFio.readUInt(in);
		unsigned int comp = mFio.readUInt(in);
		in.seekg(2*sizeof(unsigned int), ios::beg);
		float* fData = mFio.readFloatArray(in, size*comp);
		VertexArray<float>* va = new VertexArray<float>(comp, size, fData);
		va->calcBB();
		in.close();
		vbo->setVData(va, false);

		in.open(fs::path(_path / "nArray.oct"), ios::binary | ios::in);
		in.seekg(0, ios::beg);
		size = mFio.readUInt(in);
		comp = mFio.readUInt(in);
		in.seekg(2*sizeof(unsigned int), ios::beg);
		char* cData = mFio.readByteArray(in, size*comp);
		VertexArray<char>* na = new VertexArray<char>(comp, size, cData);
		in.close();
		vbo->setNData(na, false);

		cout << "reading vbo: " << vbo->getVData()->size << ", " << vbo->getVData()->mData[0] << endl;

		return vbo;
	}
}

LooseProcessingOctree*
OctreeHandler::rawToProcOctree(const fs::path& _path, const BoundingBox& _bb)
{
	bool rootIteration = false;
	if (mProcOctree == 0){
		mProcOctree = new LooseProcessingOctree(0, _bb, string(""));
		rootIteration = true;
	}
	fs::directory_iterator dir_iter(_path), dir_end;
	for (; dir_iter != dir_end; ++dir_iter) {
		if (fs::is_directory(dir_iter->status())){
			rawToProcOctree(dir_iter->path(), _bb);
		}
		else if (dir_iter->path().filename() == "vArray.bin"){
			fs::ifstream in;
			in.open(dir_iter->path(), ios::binary | ios::in);
			in.seekg(0, ios::beg);
			FileHeader fh = FileHeader();
			mFio.readHeader(fh, in);
			in.close();
			ProcessingObject* po = new ProcessingObject();
			po->bb = *fh.bb;
			po->pathName = dir_iter->path().parent_path().string();
			po->triangleCount = fh.nFaces;
			mProcOctree->insertObject(po);
		}
	}

	if (rootIteration){
		LooseProcessingOctree* temp = mProcOctree;
		mProcOctree = 0;
		return temp;
	}
	else
		return 0;
}

void
OctreeHandler::writeProcTree(LooseProcessingOctree* _tree)
{
	if (!_tree->isLeaf()) {
		writeProcTree(_tree->getFne());
		writeProcTree(_tree->getFnw());
		writeProcTree(_tree->getFse());
		writeProcTree(_tree->getFsw());
		writeProcTree(_tree->getBne());
		writeProcTree(_tree->getBnw());
		writeProcTree(_tree->getBse());
		writeProcTree(_tree->getBsw());
	}
	vector<ProcessingObject*> temp = _tree->getData();
	if (!temp.empty()){
		fs::path currentPath(_tree->getPath());
		mFio.recursiveTestAndSet(currentPath);
		fs::ofstream outFile;
		outFile.open( currentPath / "objectList.txt", ios::out);
		outFile << "Tree-Path: "<< _tree->getPath() << endl;
		outFile << "TriCount: "<< _tree->getTriangleCount() << endl;
		outFile << "Size(bytes): " << (4*sizeof(char)+4*sizeof(float))*_tree->getTriangleCount()*3 << endl;
		outFile << "TreeLevel: " << _tree->getLevel() << endl;
		outFile << "Node-BB: "<< _tree->getBb().toString() << endl;
//		V3f edges;
//		_tree->getBb().computeEdgeSizes(edges);
//		outFile << "Node-EdgeSizes: "<< edges.toString() << endl;
//		outFile << "Loose Node-BB: "<< _tree->getExtBb().toString() << endl;
//		BoundingBox tempBB;
		vector<ProcessingObject*>::const_iterator it = temp.begin();
//		for(; it != temp.end(); ++it){
//			tempBB.expand((*it)->bb);
//		}
//		outFile << "Object-BB: "<< tempBB.toString() << endl;
		for(it = temp.begin(); it != temp.end(); ++it){
			outFile << (*it)->pathName << endl;
		}
		outFile.close();
	}
}

bool
OctreeHandler::readProcTreeCell(fs::path path)
{
	fs::ifstream inFile;
	string line;
	if (path.filename() == "objectList.txt"){
		inFile.open( path, ios::in);
	}
	else if ( fs::exists(path / "objectList.txt")){
		inFile.open( path / "objectList.txt", ios::in);
	}
	else
		return 0;

	Vbo* vbo;
	while (!inFile.eof()){
		getline(inFile, line);
		if (line.substr(0, 1) == "/"){
			vbo = rmh.readRawVbo(fs::path(line));
			vbo->setOnline();
			VboManager::getSingleton()->addVbo(line, vbo);
		}
		else if (line.substr(0, 11) == "Tree-Path: "){
//			cout << "Path: " << line.substr(12) << endl;
		}
	}
	inFile.close();
	return 1;
}

bool
OctreeHandler::readProcTreeCell(fs::path path, list<Triangle>& triList)
{
	fs::ifstream inFile;
	string line;
	if (path.filename() == "objectList.txt"){
		inFile.open( path, ios::in);
	}
	else if ( fs::exists(path / "objectList.txt")){
		inFile.open( path / "objectList.txt", ios::in);
	}
	else
		return 0;

	Vbo* vbo;
	while (!inFile.eof()){
		getline(inFile, line);
		if (line.substr(0, 1) == "/"){
			vbo = rmh.readRawVbo(fs::path(line));
			const VertexArray<float>* va = vbo->getVData();
			const VertexArray<char>* na = vbo->getNData();
			for(unsigned i=0; i<va->size*va->nComponents; i+=3*va->nComponents ){
				triList.push_back(Triangle(va->mData+i, na->mData+i));
			}
			delete vbo;
		}
		else if (line.substr(0, 11) == "Tree-Path: "){
//			cout << "Path: " << line.substr(12) << endl;
		}
	}
	inFile.close();
	return 1;
}

bool
OctreeHandler::readLooseOctreeCell(fs::path path, std::string id, list<Triangle>& triList)
{
	fs::ifstream vaFile;
	fs::ifstream naFile;
	string line;
	string vaName = id;
	vaName.append(".va");
	string naName = id;
	naName.append(".na");
	// we assume we're in ../data/
	vaFile.open(path / vaName, ios::in);
	naFile.open(path / naName, ios::in);
	vaFile.seekg(0, ios::beg);
	FileHeader fh = FileHeader();
	mFio.readHeader(fh, vaFile);
	vaFile.seekg(FileHeader::getHeaderSize(), ios::beg);
	naFile.seekg(FileHeader::getHeaderSize(), ios::beg);
	float* vertices = mFio.readFloatArray(vaFile, fh.nVertices*4);
	char* normals = mFio.readByteArray(naFile, fh.nVertices*4);
	for(unsigned i =0; i<fh.nVertices*4; i+=4*3){
		triList.push_back(Triangle(vertices+i, normals+i));
	}
	delete[] vertices;
	delete[] normals;
	vaFile.close();
	naFile.close();

	return 1;
}

bool
OctreeHandler::readLooseOctreeCell(fs::path path, std::string id)
{
	fs::ifstream vaFile;
	fs::ifstream naFile;
	string line;
	string vaName = id;
	vaName.append(".va");
	string naName = id;
	naName.append(".na");
	// we assume we're in ../data/
	vaFile.open(path / vaName, ios::in|ios::binary);
	naFile.open(path / naName, ios::in|ios::binary);
	cout << (path / vaName).string() << endl;
	vaFile.seekg(0, ios::beg);
	naFile.seekg(0, ios::beg);
	FileHeader fh = FileHeader();
	mFio.readHeader(fh, vaFile);
	vaFile.seekg(FileHeader::getHeaderSize(), ios::beg);
	naFile.seekg(FileHeader::getHeaderSize(), ios::beg);
	float* vertices = mFio.readFloatArray(vaFile, fh.nFaces*3*4);
	char* normals = mFio.readByteArray(naFile, fh.nFaces*3*4);
	VertexArray<float>* va = new VertexArray<float>(4, fh.nVertices, vertices);
	va->calcBB();
	Vbo* vbo = new Vbo(0, new VertexArray<char>(4, fh.nNormals, normals), va);
	ooctools::VboManager::getSingleton()->addVbo(id, vbo);
	vaFile.close();
	naFile.close();

	return 1;
}

bool
OctreeHandler::writeLooseOctreeCell(fs::path path, std::string id, const list<Triangle>& triList, const BoundingBox& bb)
{
	fs::ofstream vaFile;
	fs::ofstream naFile;
	string line;
	string vaName = id;
	vaName.append(".va");
	string naName = id;
	naName.append(".na");

	// remove existing files
	if (fs::exists(path / vaName)){
		fs::remove(path / vaName);
		fs::remove(path / naName);
	}

	// we assume we're in ../data/
	vaFile.open(path / vaName, ios::out);
	naFile.open(path / naName, ios::out);
	vaFile.seekp(0, ios::beg);

	FileHeader fh;
	fh.bb = new BoundingBox(bb);
	fh.color = new V3ub((unsigned char)0);
	fh.nFaces = triList.size();
	fh.nNormals = fh.nFaces * 3;
	fh.nVertices = fh.nNormals;
	mFio.writeHeader(fh, vaFile);
	mFio.writeHeader(fh, naFile);
	vaFile.seekp(FileHeader::getHeaderSize(), ios::beg);
	naFile.seekp(FileHeader::getHeaderSize(), ios::beg);
	mFio.writeTriList(triList, vaFile, naFile);

	vaFile.close();
	naFile.close();

//	delete fh.bb;
//	delete fh.color;

	return 1;
}

bool
OctreeHandler::readSubProcTree(fs::path path)
{
	fs::directory_iterator dir_iter(path), dir_end;
	for (; dir_iter != dir_end; ++dir_iter) {
		if(fs::is_directory(dir_iter->status())){
			readSubProcTree(dir_iter->path());
		}
		else if (dir_iter->path().filename() == "objectList.txt"){
			readProcTreeCell(dir_iter->path());
		}
	}
	return 1;
}

bool
OctreeHandler::readSingleLevelProcTree(fs::path path, unsigned int level, unsigned int currentLevel)
{
	fs::directory_iterator dir_iter(path), dir_end;
	for (; dir_iter != dir_end; ++dir_iter) {
		if(fs::is_directory(dir_iter->status()) && currentLevel< level){
			readSingleLevelProcTree(dir_iter->path(), level, currentLevel+1);
		}
		else if (dir_iter->path().filename() == "objectList.txt"  && currentLevel == level){
			readProcTreeCell(dir_iter->path());
		}
	}
	return 1;
}

void
OctreeHandler::recursiveLpo2lo(fs::path lpoSrc, fs::path loDst, LooseOctree* lo)
{
	fs::directory_iterator dir_iter(lpoSrc), dir_end;
	vector<fs::path> pathList = vector<fs::path>();
	list<Triangle> triList = list<Triangle>();
	// create pathlist in current directory and load data
	for (; dir_iter != dir_end; ++dir_iter) {
		if (fs::is_directory(dir_iter->status())){
			pathList.push_back(dir_iter->path());
			lo->makeChild(codeMap[dir_iter->path().filename()]);
		}
		else
			if (dir_iter->path().filename() == "objectList.txt"){
			readProcTreeCell(dir_iter->path(), triList);
		}
	}

	// DEBUG
//	for(vector<fs::path>::iterator it= pathList.begin(); it!=pathList.end(); ++it){
//		cout << *it << " - " << codeMap[it->filename()] << endl;
//	}
//	if(lo->getId() == 1){
//		cout << "THERE" << endl;
//	}
//	for (unsigned i =0; i< pathList.size(); ++i){
//		cout << pathList[i] << endl;
//	}
//	cout << "sizeof trilist for node '" << lo->getIdString() << "': " << triList.size() << endl;
	// DEBUG
	// check whether there is already a saved object and load it
	string fname;
	fname.append(lo->getIdString());
	fname.append(".va");
	if (fs::exists(loDst / "data" / fname)){
		readLooseOctreeCell(loDst / "data",lo->getIdString(), triList);
	}
	if (triList.begin() != triList.end()){
		lo->insertTriList(triList);
		// save/append subtree to disk
		saveLooseSubTreeData(lo, loDst / "data");
		lo->trimSubTreeData();
	}
	// at this point we have read in all data for this node.
	// now we need to insert it into the octree,
	// apply the constraint,
	// check how the data was distributed to the level below
	// write data for this node and append data to children
	// do something
	//TODO tuts noch nicht!
	//TODO mapping erstellen: buchstaben nach child-id; ie. fnw -> 0, fnw->1 -> codeMap
	for (vector<fs::path>::iterator it = pathList.begin(); it != pathList.end(); ++it){
		lo->makeChild(codeMap[it->filename()]);
		LooseOctree* newLo = lo->getChild(codeMap[it->filename()]);
		recursiveLpo2lo(*it, loDst, newLo);
	}
	// foreach subdir recurse
}

void
OctreeHandler::lpo2lo(string lpoSrc, string loDst, const BoundingBox& scBb)
{
	fs::path lpoSrcPath(lpoSrc);
	fs::path loDstPath(loDst);
	if (!fs::exists(loDstPath)){
		fs::create_directory(loDstPath);
	}
	if (!fs::exists(loDstPath / "data")){
		fs::create_directory(loDstPath / "data");
	}
	LooseOctree* lo = new LooseOctree(0, scBb);
	recursiveLpo2lo(lpoSrcPath, loDstPath, lo);
	// saving skeleton
	fs::ofstream skelFile;
	skelFile.open(loDstPath / "skeleton.bin", ios::out);
	skelFile.seekp(0, ios::beg);
	saveLooseOctreeSkeleton(lo, skelFile);
	skelFile.close();

	// TODO
	delete lo;

}

void
OctreeHandler::saveLooseSubTreeData(LooseOctree* lo, fs::path path)
{
	std::list<Triangle> tris = lo->getData();
	string filename = lo->getIdString();
	filename.append(".va");
	if(fs::exists(path / filename)){
		// delete file
		fs::remove(path / filename);
		filename[filename.size()-2] = 'n';
		fs::remove(path / filename);
	}

	if (tris.begin() != tris.end())
		writeLooseOctreeCell(path, lo->getIdString(),tris,lo->getBb());
//TODO
	LooseOctree* child;
	for(unsigned i =0; i< 8; ++i){
		child = lo->getChild(i);
		if (child!=0 && !child->isEmpty()){
			// recurse
			saveLooseSubTreeData(child, path);
		}
	}
}

void
OctreeHandler::saveLooseOctreeSkeleton(LooseOctree* lo, fs::ofstream& _of)
{
	mFio.writeLooseOctreeNodeSkeleton(lo, _of);
	uint64_t pos = _of.tellp();
	_of.seekp(pos+LooseOctree::getSkeletonSize());
	for(unsigned i=0; i<8; ++i){
		if (lo->getChild(i) != 0 )
			saveLooseOctreeSkeleton(lo->getChild(i), _of);
	}
}

LooseOctree*
OctreeHandler::loadLooseOctreeSkeleton(fs::path file)
{
	fs::ifstream _if;
	_if.open(file, ios::in|ios::binary);
	LooseOctree* lo;
	_if.seekg (0, ios::end);
	unsigned length = _if.tellg();
	_if.seekg (0, ios::beg);


	lo = mFio.readLooseOctreeNodeSkeleton(_if);
	LooseOctree::maxTriCount = lo->getTriangleCount();
	LooseOctree::totalTriCount += lo->getTriangleCount();

	//DEBUG
	cout << "reading id: " << lo->getIdString() << endl;
//	if (lo->getId() == 1)
//	{
//		cout << lo->getIdString() << endl;
//	}
	//DEBUG
	unsigned pos = _if.tellg();
	_if.seekg(pos+LooseOctree::getSkeletonSize(), ios::beg);

	while(pos < length){
		lo->insertNode(mFio.readLooseOctreeNodeSkeleton(_if));
		pos = _if.tellg();
		_if.seekg(pos+LooseOctree::getSkeletonSize(), ios::beg);
	}

	cout << "MaxLevel of OctreeSkel: " << LooseOctree::maxLevel << endl;
	cout << "MaxTriCount/node of OctreeSkel: " << LooseOctree::maxTriCount << endl;
	cout << "TotalTriCount: " << LooseOctree::totalTriCount << endl;
	for (unsigned i =0; i< 8; ++i){
		cout << "Children with id " << i << ": " << LooseOctree::descendantCount[i] << endl;
	}
//	exit(0);


	return lo;
}

void
OctreeHandler::convertProctreeCell(fs::path lpoCell, fs::path dstDir, string id, LooseOctree* lo)
{
	fs::ifstream inFile;
	TriangleStream tsOut;

	tsOut.open(dstDir.string()+"/"+id, TriangleStream::OUT, TriangleStream::APPEND);
//	cout<< dstDir.string()+"/"+id << endl;
	string line;
	if (lpoCell.filename() == "objectList.txt"){
		inFile.open( lpoCell, ios::in);
	}
	else if ( fs::exists(lpoCell / "objectList.txt")){
		inFile.open( lpoCell / "objectList.txt", ios::in);
	}

	while (!inFile.eof()){
		getline(inFile, line);
		if (line.substr(0, 1) == "/"){
			Vbo* vbo;
			vbo = rmh.readRawVbo(fs::path(line));
			const VertexArray<float>* va = vbo->getVData();
			const VertexArray<char>* na = vbo->getNData();
			for(unsigned i=0; i<va->size*va->nComponents; i+=3*va->nComponents ){
				Triangle tri(va->mData+i, na->mData+i);
				tsOut.write(tri);
				if (lo != 0){
					lo->addTriangleArea(tri.getArea());
					lo->incTriCount();
				}
			}
			delete vbo;
			vbo = 0;
		}
	}
	if (!tsOut.checkHeaderVsFile()){
		cerr << "header/filesize mismatch in OctreeHandler::convertProctreeCell" << endl;
		cerr << "lpocell: " << lpoCell.string() << endl;
		cerr << "dstDir: " << dstDir.string() << endl;
		cerr << "id: " << id << endl;
		cerr << "-------------------------------------------------" << endl;
	}
	inFile.close();
	tsOut.close();
}

void
OctreeHandler::recursiveProc2Octree(fs::path lpoSrc, fs::path dstDir, LooseOctree* lo)
{
	if (lo == 0){
		lo = new LooseOctree(0,BoundingBox::fromFile(fs::path("/media/ClemensHDD/SceneBoundingBox.bin")), 0);
	}

	fs::directory_iterator dir_iter(lpoSrc), dir_end;
	vector<fs::path> pathList = vector<fs::path>();
	// create pathlist in current directory and load data
	for (; dir_iter != dir_end; ++dir_iter) {
		if (fs::is_directory(dir_iter->status())){
			pathList.push_back(dir_iter->path());
			lo->makeChild(codeMap[dir_iter->path().filename()]);
		}
		else
			if (dir_iter->path().filename() == "objectList.txt"){
				convertProctreeCell(dir_iter->path(), dstDir / "data", lo->getIdString(), lo);
			}
	}

	for (vector<fs::path>::iterator it = pathList.begin(); it!=pathList.end(); ++it){
//		cout << "tes:" << *it << endl;
//		cout << fs::exists(*it) << endl;
		LooseOctree* newLo = lo->getChild(codeMap[it->filename()]);
		recursiveProc2Octree(*it, dstDir, newLo);
	}

	if (lo->getId() == 0){
		// save Skel
		fs::ofstream skelFile;
		string test(fs::path(dstDir/"skeleton.bin").string());
		skelFile.open(dstDir/"skeleton.bin", ios::out|ios::binary);
		saveLooseOctreeSkeleton(lo, skelFile);
		skelFile.close();
	}
}

void
OctreeHandler::recursiveOct2LooseOct(fs::path ocPath, fs::path lOcPath, LooseOctree* lo)
{
	TriangleStream childStream[9];
	TriangleStream tsIn, tsOut;
	if (lo == 0){
		lo = loadLooseOctreeSkeleton(ocPath / "skeleton.bin");
	}

	// if there is already a datanode in the target-directory
	// append the "old" one to the new and correct
	if (fs::exists(lOcPath/ ("data/"+lo->getDirPrefix()+lo->getIdString()+".va")) && fs::exists(ocPath/ ("data/"+lo->getIdString()+".va"))){
		tsIn.open(ocPath / ("data/"+lo->getIdString()+".va"), TriangleStream::IN);
		tsOut.open(lOcPath / ("data/"+lo->getDirPrefix()+lo->getIdString()+".va"), TriangleStream::OUT, TriangleStream::APPEND);
		Triangle tri = Triangle();
		while(tsIn.read(tri)){
			tsOut.write(tri);
		}
		tsIn.close();
		tsOut.close();

		// set the new values for the current node
		lo->setTriCount(0);
		lo->setAreaSum(0.0);
		if (lo->getId() == 1){
			cout << endl;
		}
		tsIn.open(lOcPath / ("data/"+lo->getDirPrefix()+lo->getIdString()+".va"), TriangleStream::IN);
		while(tsIn.read(tri)){
			lo->incTriCount();
			lo->addTriangleArea(tri.getArea());
		}
		if(lo->getTriangleCount() != tsIn.getTriangleSize()){
			cerr << "counted no of tris (" << lo->getTriangleCount() << ") != trinumber in stream! (" << tsIn.getTriangleSize() << ") [" << lo->getIdString() << "]" << endl;
			exit(0);
		}
		tsIn.close();
	}
	// if there's only an "old" data node
	// just copy the file - the skeleton-values should still be valid.
	else if (fs::exists(ocPath/ ("data/"+lo->getIdString()+".va"))){
		mFio.recursiveTestAndSet(lOcPath/ ("data/"+lo->getDirPrefix()));
		fs::copy_file(ocPath/ ("data/"+lo->getIdString()+".va"), lOcPath/ ("data/"+lo->getDirPrefix()+lo->getIdString()+".va"));
		fs::copy_file(ocPath/ ("data/"+lo->getIdString()+".na"), lOcPath/ ("data/"+lo->getDirPrefix()+lo->getIdString()+".na"));
	}
	// if there's only a "new" data-node
	// reset this nodes values and read in the triCount and the AreaSum
	else if (fs::exists(lOcPath/ ("data/"+lo->getDirPrefix()+lo->getIdString()+".va"))){
		tsIn.open(lOcPath / ("data/"+lo->getDirPrefix()+lo->getIdString()+".va"), TriangleStream::IN);
		Triangle tri = Triangle();
		lo->setTriCount(0);
		lo->setAreaSum(0.0);
		while(tsIn.read(tri)){
			lo->incTriCount();
			lo->addTriangleArea(tri.getArea());
		}
		tsIn.close();
	}

	// there shouldn't be another else, because that'd mean we're at an empty node
	// with no data at all!

	// if there's data in the current node, check for holding of condition
	if (lo->hasData() && !lo->conditionHolds()){
		tsIn.open(lOcPath / ("data/"+lo->getDirPrefix()+lo->getIdString()+".va"), TriangleStream::IN);
		Triangle tri = Triangle();
		lo->setTriCount(0);
		lo->setAreaSum(0.0);
		// read each triangle in and test for insertion-point
		while(tsIn.read(tri)){
			unsigned childId = lo->findInsertionPoint(tri);
			if (!childStream[childId].isOpen()){
				if (childId == 8){
					childStream[childId].open(lOcPath / ("data/"+lo->getDirPrefix()+lo->getIdString()+"_.va"), TriangleStream::OUT, TriangleStream::APPEND);
				}
				else {
					mFio.recursiveTestAndSet(lOcPath/ ("data/"+lo->getChild(childId)->getDirPrefix()));
					childStream[childId].open(lOcPath / ("data/"+lo->getChild(childId)->getDirPrefix()+lo->getChild(childId)->getIdString()+".va"), TriangleStream::OUT, TriangleStream::APPEND);
				}
			}
			childStream[childId].write(tri);

			// the triangle stays.
			if (childId==8){
				lo->incTriCount();
				lo->addTriangleArea(tri.getArea());
			}
		}
		tsIn.close();

		// remove original "new" data-node
		fs::remove(lOcPath / ("data/"+lo->getDirPrefix()+lo->getIdString()+".va"));
		fs::remove(lOcPath / ("data/"+lo->getDirPrefix()+lo->getIdString()+".na"));
		for(unsigned i=0; i<9; ++i){
			if (childStream[i].isOpen()){
				childStream[i].close();
				if (i==8){
					// do we have a "new" "new" data-node?
					// the rename it
					fs::rename(lOcPath / ("data/"+lo->getDirPrefix()+lo->getIdString()+"_.va"), lOcPath / ("data/"+lo->getDirPrefix()+lo->getIdString()+".va"));
					fs::rename(lOcPath / ("data/"+lo->getDirPrefix()+lo->getIdString()+"_.na"), lOcPath / ("data/"+lo->getDirPrefix()+lo->getIdString()+".na"));
				}
			}
		}
	}

	// set statistical data
	(lo->getTriangleCount()>LooseOctree::maxTriCount) ? LooseOctree::maxTriCount= lo->getTriangleCount() : LooseOctree::maxTriCount;
	if (!lo->isRoot()){
		LooseOctree::descendantCount[lo->getId()-1 - (lo->getFather()->getId()*8)]+=1;
	}

	// new recurse this procedure for each existing child
	for (unsigned i=0; i<8; ++i){
		LooseOctree* newLo = lo->getChild(i);
		if (newLo != 0){
			recursiveOct2LooseOct(ocPath, lOcPath, newLo);
		}
	}


	// save skeleton and be done
	if (lo->getId() == 0){
		// save Skel
		fs::ofstream skelFile;
		skelFile.open(lOcPath/"skeleton.bin", ios::out|ios::binary);
		saveLooseOctreeSkeleton(lo, skelFile);
		skelFile.close();

		// echo statistical data
		cout << "max number of Tris in a single node: " << LooseOctree::maxTriCount << endl;
		for(unsigned i=0; i<8; ++i){
			cout << "Anzahl "<< i << " Kinder: " << LooseOctree::descendantCount[i] << endl;
		}
	}
}

bool
OctreeHandler::loadSubtreeData(int64_t id, LooseOctree* node, bool foundNode)
{
	if (node == 0){
		return false;
	}

	if (foundNode){
		if (node->hasData()){
			readLooseOctreeCell(fs::path("/media/ClemensHDD/B3_LooseOctree_tris/data/"+node->getDirPrefix()), node->getIdString());
		}
		if (node->isLeaf()){
			return true;
		}
	}

	if (node->getId() == id){
		for (unsigned i=0; i<8; ++i){
			loadSubtreeData(id, node->getChild(i), true);
		}
	}
	else {
		for (unsigned i=0; i<8; ++i){
			loadSubtreeData(id, node->getChild(i), foundNode);
		}
	}
	return true;
}

void
OctreeHandler::updateSkeleton(LooseOctree* tree, fs::path path)
{
	///////////////////////////////////////////////////
	fs::ifstream in;
	in.open(path, ios::binary);
	unsigned size = 0;
	in.seekg(0, ios::beg);
	in.read((char*)&size, sizeof(unsigned));
	in.seekg(sizeof(unsigned), ios::beg);
	unsigned idxes[size];
	in.read((char*)idxes, size*sizeof(unsigned));
	for(unsigned i=0; i<size; i+=3){
		cout << "IDX: " << idxes[i] << ", " << idxes[i+1] << ", " << idxes[i+2] << endl;
	}
	in.close();
	///////////////////////////////////////////////////

	if (tree->getId() == 11){
		cout << endl;
	}
	if (tree->hasData()){
		if (!fs::exists(path / (tree->getDirPrefix()+tree->getIdString()+".idx"))){
			cout << "The file " << path << (tree->getDirPrefix()+tree->getIdString()+".idx") << " does not exist!" << endl;
		}
		else {
			fs::ifstream idxFile;
			idxFile.open(path / (tree->getDirPrefix()+tree->getIdString()+".idx"), ios::binary);
			unsigned idxSize = 0;
			idxFile.read((char*)&idxSize,sizeof(unsigned));
			idxFile.close();
			if (idxSize/3 != tree->getTriangleCount()){
				cout << "# floats in file: " << idxSize << endl;
				cout << "# tris in skel: " << tree->getTriangleCount() << endl;
				cout << "# tris in file: " << idxSize/3 << endl;
				cout << "----------------------------------" << endl;
				tree->setTriCount(idxSize/3);
				tree->setAreaSum(calcAreaSumFromPath(path / tree->getDirPrefix(), tree->getIdString()));
			}
		}
	}

	if (!tree->isLeaf()){
		LooseOctree* child = 0;
		for (unsigned i=0; i<8; ++i){
			child = tree->getChild(i);
			if (child !=0){
				updateSkeleton(child, path);
				child =0;
			}
		}
	}
}

double
OctreeHandler::calcAreaSumFromPath(fs::path path, std::string id)
{
	cout << "Fixing path " << path << "/" << id << endl;
	unsigned vertexSize = 0;
	unsigned indexSize = 0;
	fs::ifstream dataFile;
	fs::ifstream idxFile;
	dataFile.open(path / (id+".data"), ios::binary);
	idxFile.open(path / (id+".idx"), ios::binary);
	dataFile.seekg(0, ios::beg);
	idxFile.seekg(0, ios::beg);
	dataFile.read((char*)&vertexSize, sizeof(unsigned));
	idxFile.read((char*)&indexSize, sizeof(unsigned));
	dataFile.seekg(sizeof(unsigned), ios::beg);
	idxFile.seekg(sizeof(unsigned), ios::beg);
	V4N4 vertexArray[vertexSize];
	dataFile.read((char*)&vertexArray, sizeof(V4N4)*vertexSize);
	unsigned indexArray[indexSize];
	idxFile.read((char*)&indexArray, sizeof(unsigned)*indexSize);
	dataFile.close();
	idxFile.close();

	double areaSum = 0.0;
	Triangle tri = Triangle();
	for(unsigned i =0; i<indexSize;i+=3){
		if (i==54){
			cout << endl;
		}
		tri = Triangle(vertexArray[indexArray[i]], vertexArray[indexArray[i+1]], vertexArray[indexArray[i+2]]);
		areaSum+=tri.getArea();
	}

	return areaSum;
}

void
OctreeHandler::generateIdPathMap(const LooseOctree* lo, std::map<uint64_t, std::string>& idPathMap) const
{
	// if node has triangles, grab the reference
	if (lo->hasData()){
		std::string s = string(lo->getDirPrefix());
		s.append(lo->getIdString());
		idPathMap.insert(make_pair(lo->getId(), s));
	}
	// process children
	for (unsigned i=0; i<8; ++i){
		if (lo->getChild(i)!=0){
			generateIdPathMap(lo->getChild(i), idPathMap);
		}
	}
}

void
OctreeHandler::generateIdLoMap(LooseOctree* lo, std::map<uint64_t, oocformats::LooseOctree*>& idLoMap) const
{
	if (lo->getId()==96){
		cout << "128 has data? " << lo->hasData() << endl;
		cout << "128 is empty? " << lo->isEmpty() << endl;
		cout << "128 is leaf? " << lo->isLeaf() << endl;
		cout << "128 path: " << lo->getDirPrefix() << endl;
		exit(0);
	}

	// if node has triangles, grab the reference
	if (lo->hasData()){
		idLoMap.insert(make_pair(lo->getId(), lo));
	}
	// process children
	for (unsigned i=0; i<8; ++i){
		if (lo->getChild(i)!=0){
			generateIdLoMap(lo->getChild(i), idLoMap);
		}
	}
}

} // oocformats
