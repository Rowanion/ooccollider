//============================================================================
// Name        : ModelConverter.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


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
#include "OOCTools.h"
#include <cmath>
#include <sstream>
#include <map>

#include "boost/filesystem.hpp"
#include <boost/filesystem/fstream.hpp>

#include "GlToolkit.h"

#include "ObjModelLoader.h"
#include "ModelWriter.h"
#include "RawModelWriter.h"
#include "OctreeModelWriter.h"
#include "ColorTable.h"

#include "../../OOCFormats/include/declarations.h"

using namespace std;
using namespace ooctools;
using namespace oocformats;

Model *model;
Model *model2;
V3f tri1;
V3f tri2;
V3f tri3;

bool vboSwitch= true;
bool showFPS = true;
bool showBBox = false;
bool showLightSource = true;
float zoom = 1.0f;
int mouseLastX, mouseLastY;
float mouseRotX = 0.0f;
float mouseRotY = 0.0f;
int vertexCount = 0;
float transX = 0.0;
float transY = 0.0;
float transZ = 0.0;
float modelScale = 1.0f;

VboManager *vboMan;
Fbo *fbo;

RawModelWriter *moWri;

// frame - the number of frames since we last computed the frame rate
// time - the current number of milliseconds
// timebase - the number of milliseconds since we last computed the frame rate
int frame=0,time_passed,timebase=0;
double fps = 0.0;
int w=512, h=512;

ObjModelLoader moLoader;

static void display() {
	glClearColor(0.0f,0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);


	glPushMatrix();
		glTranslatef(transX, 0.0, transZ);
		glPushMatrix();

			glScalef(zoom, zoom, zoom);
			glScalef(modelScale,modelScale,modelScale);
			glRotatef(mouseRotX, 1.0f, 0.0, 0.0);
			glRotatef(mouseRotY, 0.0f, 1.0, 0.0);
			glPushMatrix();
				if (showBBox) {
					vboMan->getBb()->draw(0,255,0);
					vboMan->drawBbs(255,0,0);
				}
//				vboMan->drawNormals();

				// enable profiles

				//glColor4ub(222,143,28,1);


				//drawTriangle();
				//drawObj();
				//drawObj2();
				//renderBuffers2();
				//renderBuffers2();
				//drawOldSchool(model);


//				for(map<string, VBO*>::iterator it = vboList->begin(); it!= vboList->end(); ++it){
//					it->second->draw(10);
//				}
				glColor3f(0.0f, 1.0f, 0.0f);
				glBegin(GL_TRIANGLES);
					glVertex3fv(tri1.getData());
					glVertex3fv(tri2.getData());
					glVertex3fv(tri3.getData());
					glVertex3fv(tri3.getData());
					glVertex3fv(tri2.getData());
					glVertex3fv(tri1.getData());
				glEnd();

//				fbo->bind();
//				fbo->clear();
				vboMan->drawVbos();
//				vboMan->drawVbo("robot");
//				vboMan->drawVbo("bunny");
//				glGetFloatv (GL_MODELVIEW_MATRIX, mvMatrix);
//				OOCTools::readColorFB(pixels, 0,0, w,h);
//				fbo->unbind();


				//vboMan->drawVBO("defaultGrp");

				//vbo.draw(60);
				//vbo2.draw(60);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();

	//glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
//	glWindowPos2i(0,0);
//	glPushAttrib(GL_ALL_ATTRIB_BITS);
//	//glDisable(GL_DEPTH_TEST);
//	glDepthFunc(GL_ALWAYS);
//	glDisable(GL_CULL_FACE);
//	glDrawPixels(w,h,GL_DEPTH_COMPONENT, GL_FLOAT, depth);
//	glDrawPixels(w,h,GL_BGRA, GL_UNSIGNED_BYTE, pixels);
//	glPopAttrib();
	/*
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, fbo->getColorTexId());
	glBegin(GL_QUADS);
		glNormal3f( 0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f,  1.0f);
	glEnd();

	glDisable(GL_TEXTURE_2D);
*/
	//glutPostRedisplay();
//	exit(0);
	glutSwapBuffers();
	//glFlush();
}




static void glInit(int argc, char *argv[]){
	//readObj("meshes/cow.obj");
	getGlError(0);
	glutInit(&argc, argv);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutInitWindowSize(w,h);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow("ModelConverter");
	glewInit();
	glutDisplayFunc(display);

	getGlError(0);

	vboMan->makeVbos(model);

//	moWri->readModel(fs::path("../ModelViewer/raw_objs/budda")); // 3,2 Mil. / 1,08 Mil.
//	moWri->readModel(fs::path("../ModelViewer/raw_objs/dragon")); //2,6 Mil. / 871k
//	moWri->readModel(fs::path("../ModelViewer/raw_objs/armadillo")); // 1,03 Mil. / 345k
//	moWri->readModel(fs::path("../ModelViewer/raw_objs/mini")); //912k / 304k
//	moWri->readModel(fs::path("../ModelViewer/raw_objs/bunny")); //48k / 16k
//	moWri->readModel(fs::path("../ModelViewer/raw_objs/shadow")); //30k / 10k
//	moWri->readModel(fs::path("../ModelViewer/raw_objs/door")); //25k / 8k
//	moWri->readModel(fs::path("../ModelViewer/raw_objs/beethoven")); //15k / 5k
//	moWri->readModel(fs::path("../ModelViewer/raw_objs/robot")); //3,6k / 1,2k
	vboMan->printInfo();

	getGlError(0);
}

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
		RawModelLoader* rawLoader = 0;
		RawModelWriter* rawWriter = 0;
		OctreeModelWriter* octWriter = 0;
		ColorTable* ct = 0;

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
			else if (srcState == IS_OBJ_FILE) {
				cout << "src is file" << endl;
				ct = new ColorTable(cTable);
				objLoader = new ObjModelLoader(ct);
				model = objLoader->parseMultipass(src.string(), true);
				model->setColorTable(*ct);
				rawWriter = new RawModelWriter();
				string objName(src.filename().substr(0, src.filename().size()-4));
				if (!fs::exists(fs::path(dst / objName)))
					fs::create_directory(fs::path(dst / objName));
				rawWriter->writeModel(model, fs::path(dst / objName));
				// TODO fix nested model destructor
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
	vboMan = VboManager::getInstancePtr();


//	modelScale = 1.0f/((fabs(model->getMaxBB()->getX()) +	fabs(model->getMinBB()->getX()))/2.0);
//	modelScale = fmax(modelScale, 1.0f/((fabs(model->getMaxBB()->getY()) + fabs(model->getMinBB()->getY()))/2.0));
//	modelScale = fmax(modelScale, 1.0f/((fabs(model->getMaxBB()->getZ()) + fabs(model->getMinBB()->getZ()))/2.0));
//	cout << "ModelScale: " << modelScale << endl;
		//fabs(model->getFirstObj()->maxV.x) + fabs(model.getLastObj()->minV.x);

//	exit(0);



	glInit(argc, argv);

	return 0;
}


