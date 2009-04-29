/**
 * @file	ModelConverter.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 23.03.2009
 *
 */

#include <iostream>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <cstddef>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <map>

#include "boost/filesystem.hpp"
#include <boost/filesystem/fstream.hpp>

#include "OOCTools.h"
#include "ObjModelLoader.h"
#include "RawModelHandler.h"
#include "OctreeHandler.h"
#include "ColorTable.h"
#include "LooseProcessingOctree.h"

#include "../../OOCFormats/include/declarations.h"

using namespace std;
using namespace ooctools;
using namespace oocformats;

#define COLOR_TABLE "/home-e/ava/Diplom/Model/colortable.bin"
#define BOUNDING_BOX "/home-e/ava/Diplom/Model/SceneBoundingBox.bin"

enum PathState
{
    IS_ERROR = 0,
    IS_EMPTY_DIR,
    IS_NONEMPTY_DIR,
    IS_OTHER_FILE,
    IS_NONEXISTENT,
    IS_OBJ_FILE
};

PathState
checkPath(const fs::path& _path)
{
	if (fs::exists(_path)){
		if (fs::is_directory(_path)){
			if (fs::is_empty(_path))
				return IS_EMPTY_DIR;
			else
				return IS_NONEMPTY_DIR;
		}
		else {
			if (fs::extension(_path)==".obj")
				return IS_OBJ_FILE;
			else
				return IS_OTHER_FILE;
		}
	}
	else {
		return IS_NONEXISTENT;
	}
}

void
printProgramInfo(){
	cout << "usage: 'ModelConverter mode source destination ColorTable'" << endl << endl;
	cout << "mode:\t\tobj2raw - converts obj file to binary format." << endl;
	cout << "\t\traw2oct - converts binary object to octree-format." << endl;
	cout << "colortable:\ta binary file that contains a colorLUT compliant with ooctools::ColorTable.h." << endl << endl;
}

void
testAndSetDir(fs::path _p)
{
	if (!exists(_p)) {
		fs::create_directory(_p);
	}
}

int main(int argc, char *argv[]) {
	if (argc != 5){
		printProgramInfo();
		return (0);
	}
	else {
		string mode(argv[1]);
		fs::path src(argv[2]);
		fs::path dst(argv[3]);
		fs::path cTable(argv[4]);
		PathState srcState = checkPath(src);
		PathState dstState = checkPath(dst);
		PathState ctState = checkPath(cTable);
		Model* model = 0;
		ObjModelLoader* objLoader = 0;
		RawModelHandler* rawWriter = 0;
		OctreeHandler* octHandler = 0;
//		ColorTable ct();

		if (ctState != IS_OTHER_FILE){
			cerr << "There seems to be a problem with the colortable you specified!" << endl;
			return 0;
		}
		if (mode == "obj2raw") {
			if (srcState == IS_NONEXISTENT) {
				cerr << "Error: source '" << src << "' does not exist!" << endl
						<< endl;
				printProgramInfo();
				return 0;
			}

			// TODO the following branch is not exception-safe!!!
			// exception is thrown when trying to create a dir within a non-existing dir.
			if (dstState == IS_NONEXISTENT && fs::create_directory(dst)) {
				cout << "Created non-existent destination-dir '" << dst << "'."
						<< endl;
			}
			else if (dstState == IS_OTHER_FILE || dstState == IS_OBJ_FILE) {
				cerr << "Error: destination '" << dst
						<< "' is a file. Needs to be a directory!" << endl
						<< endl;
				printProgramInfo();
				return 0;
			}
			else if (dstState == IS_NONEMPTY_DIR) {
				cerr << "Error: destination '" << dst
						<< "' is not empty. Please specify an empty directory!"
						<< endl << endl;
				printProgramInfo();
				return 0;
			}

			if (srcState == IS_EMPTY_DIR) {
				cerr << "Error: source '" << src
						<< "' is empty. No Objects to read from!" << endl
						<< endl;
				printProgramInfo();
				return 0;
			}
			// converting multiple objs to multiple raws
			if (srcState == IS_NONEMPTY_DIR) {
//				testAndSetDir(dst / "411W2468-1>411W2468-1_Geometry_0");
				ColorTable ct(cTable);
				ct.writeToFile(dst / "colortable.bin");
				objLoader = new ObjModelLoader(ct);
				rawWriter = new RawModelHandler();
				// TODO read whole bunch of object-files
				// modify writeModel that the colortable is not saved and
				// and the group-dirs are written directly into the given dir
				fs::directory_iterator dir_iter(src), dir_end;
				for (; dir_iter != dir_end; ++dir_iter) {
					if (fs::is_directory(dir_iter->status())){
						testAndSetDir(dst / dir_iter->path().filename());
						// dive one level deeper - a.k.a. 'Part1'
						fs::directory_iterator deeper_iter(dir_iter->path());
						for (; deeper_iter != dir_end; ++deeper_iter) {
							if (fs::is_regular_file(deeper_iter->status())
									&& deeper_iter->path().extension()==".obj"){
								model = objLoader->parseMultipass(deeper_iter->path().string(), false);
								rawWriter->writeModel(model, dst / dir_iter->path().filename() / deeper_iter->path().stem());
								delete model;
								model = 0;
							}
						}
					}
					else if (dir_iter->path().extension()==".obj"){

					}
				}
//				parse directory
//				if found another dir
//					create same dir in dst
//					recurse!
//					keep track of the directory depth!
//				if found obj-file
//					create a dir with filename-obj in dst
//					call WriteModel with this new dir as dst
				delete rawWriter;
				rawWriter = 0;
				delete objLoader;
				objLoader = 0;
				return 1;
			}
			// converting a single obj to raw-format
			else if (srcState == IS_OBJ_FILE) {
				cout << "src is file" << endl;
				ColorTable ct(cTable);
				objLoader = new ObjModelLoader(ct);
				model = objLoader->parseMultipass(src.string(), true);

//				model->setColorTable(ct);
				rawWriter = new RawModelHandler();
				string objName(src.filename().substr(0, src.filename().size()-4));
				if (!fs::exists(fs::path(dst / objName)))
					fs::create_directory(fs::path(dst / objName));
				rawWriter->writeModel(model, fs::path(dst / objName));
				delete model;
				model = 0;
				delete rawWriter;
				rawWriter = 0;
				delete objLoader;
				objLoader = 0;
				return 1;
			}
			else if (srcState == IS_OTHER_FILE) {
				cerr << "Error: source '" << src << "' is not an object-file!"
						<< endl << endl;
				printProgramInfo();
				return 0;
			}
			else if (srcState == IS_NONEMPTY_DIR) {
				cout << "src is nonempty dir" << endl;
			}
			cout << "obj2raw" << endl;
		}
		else if (mode == "raw2oct") {
			cout << "raw2oct" << endl;
			octHandler = new OctreeHandler();
			if (srcState == IS_NONEMPTY_DIR){
				if (dstState == IS_NONEXISTENT){
					fs::create_directory(dst);
				}
				BoundingBox bb = BoundingBox::fromFile(fs::path("/media/ClemensHDD/SceneBoundingBox.bin"));
				// call recursive method in OctreeHandler
				octHandler->generateOctree(src, dst, bb);
			}
			delete octHandler;
		}
		else if (mode == "raw2lpo") { // looseprocessingoctree
			cout << "raw2lpo" << endl;
			octHandler = new OctreeHandler();
			LooseProcessingOctree* lpo;
			lpo = octHandler->rawToProcOctree(fs::path("/media/Titanstab/B3_ausschnitt"), BoundingBox::fromFile(BOUNDING_BOX));
			lpo->setBasePath(string("/tmp/model/loctree"));
			octHandler->writeProcTree(lpo);

			delete lpo;
			delete octHandler;
		}
		else {
			printProgramInfo();
			return 0;
		}

	}
	for(int i=0; i< argc; ++i){
		cout << "Argument No. "<< i << ": " << argv[i] << endl;
	}
//	return (0);

// theory: parse obj into Model* - then write it with Phase1ModelWriter
// then delete model;
//	model = moLoader.parseMultipass("../ModelViewer/meshes/C141T4001S01-BD-1V4.obj", true);
//	model = moLoader.parseMultipass("../ModelViewer/meshes/osg.obj", true);
//	model = moLoader.parseMultipass("../ModelViewer/meshes/robot.obj", true);
//	model = moLoader.parseMultipass("../ModelViewer/meshes/bunny.obj", true);
//	model = moLoader.parseMultipass("../ModelViewer/meshes/happy_buddha.obj", true);
//	model = moLoader.parseMultipass("../ModelViewer/meshes/Dragon.obj", true);
//	model = moLoader.parseMultipass("../ModelViewer/meshes/mini_obj2.obj", true);
//	model = moLoader.parseMultipass("../ModelViewer/meshes/ShadowBattleCrab.obj", true);
//	model = moLoader.parseMultipass("../ModelViewer/meshes/Armadillo.obj", true);
//	model = moLoader.parseMultipass("../ModelViewer/meshes/beethoven_tri3.obj", true);
//	model->calculateNormals();
//	moWri = new Phase1ModelWriter();
//	moWri->writeModel(model, fs::path("raw_objs/budda"));
//	exit(0);
//	vboMan = VboManager::getInstancePtr();


//	modelScale = 1.0f/((fabs(model->getMaxBB()->getX()) +	fabs(model->getMinBB()->getX()))/2.0);
//	modelScale = fmax(modelScale, 1.0f/((fabs(model->getMaxBB()->getY()) + fabs(model->getMinBB()->getY()))/2.0));
//	modelScale = fmax(modelScale, 1.0f/((fabs(model->getMaxBB()->getZ()) + fabs(model->getMinBB()->getZ()))/2.0));
//	cout << "ModelScale: " << modelScale << endl;
		//fabs(model->getFirstObj()->maxV.x) + fabs(model.getLastObj()->minV.x);

//	exit(0);

	return 0;
}


