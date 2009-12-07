/*
 * ObjModelLoader.cpp
 *
 *  Created on: 02.02.2009
 *      Author: ava
 */

#include "ObjModelLoader.h"

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <sstream>

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>

#include "BoundingBox.h"
#include "Face.h"
#include "GlToolkit.h"
#include "MetaGroup.h"
#include "Model.h"
#include "StructDefs.h"
#include "V3b.h"

#include "../../OOCTools/include/declarations.h"
#include "declarations.h"

namespace fs = boost::filesystem;
using namespace std;
using namespace ooctools;

namespace oocformats {

// TODO Taking care of groups, materials and the thus encapsulated hierarchy
ObjModelLoader::ObjModelLoader() :
	mPriModelPtr(0), mPriFName("")
{
}

ObjModelLoader::ObjModelLoader(const ColorTable& ct) :
	mPriModelPtr(0), mPriFName(""), mCTable(ct)
{
}

ObjModelLoader::~ObjModelLoader()
{
	cleanup();
}

bool
ObjModelLoader::space(char c)
{
	return isspace(c);
}

bool
ObjModelLoader::no_space(char c)
{
	return !isspace(c);
}

bool
ObjModelLoader::slash(char c)
{
	return c == '/';
}

bool
ObjModelLoader::no_slash(char c)
{
	return c != '/';
}

vector<string>
ObjModelLoader::splitSpace(const string& str)
{
	typedef string::const_iterator iter;
	vector<string> result;
	iter i = str.begin();
	while (i != str.end()) {
		i = find_if(i, str.end(), ObjModelLoader::no_space);
		iter j = find_if(i, str.end(), ObjModelLoader::space);
		if (i != str.end())
			result.push_back(string(i, j));
		i = j;
	}
	return result;
}

vector<string>
ObjModelLoader::splitSlash(const string& str)
{
	typedef string::const_iterator iter;
	vector<string> result;
	iter i = str.begin();
	while (i != str.end()) {
		i = find_if(i, str.end(), ObjModelLoader::no_slash);
		iter j = find_if(i, str.end(), ObjModelLoader::slash);
		if (i != str.end())
			result.push_back(string(i, j));
		i = j;
	}
	return result;
}

vector<int>
ObjModelLoader::extractFaceComponents(const string& s)
{
	/*
	 * Bitfield for components:
	 * 1 0 1	(example) vertex and normal present but no texture coords
	 */
	int bitField = 0;
	vector<int> result(4);
	//cout << "Original: " << s << endl;
	//cout << "Kopie   : ";

	string token;
	istringstream isstream(s);
	int componentCount = 0;
	while (getline(isstream, token, '/')) {
		if (!token.empty()) {
			bitField |= (int) (pow((double)2, (double)2 - componentCount));
			result[componentCount + 1] = atoi(token.c_str());
			//cout <<  result[componentCount+1] << "/";
		}
		//else cout << "/";

		++componentCount;
	}
	//cout << endl;
	//cout << "-------" << bitField << "-------------" << endl;
	result[0] = bitField;
	return result;
}

Model*
ObjModelLoader::parseMultipass(string _fname, bool verbose)
{
	mPriFName.assign(_fname);

	mPriModelPtr = new Model();
	firstPass();

	secondPass();
	mPriModelPtr->resetGrpPtr();
	if (verbose) {
		cout << "Loaded model '" << mPriFName << "' with "
				<< mPriModelPtr->getFaceCount() << " # of Triangles in "
				<< mPriModelPtr->getGrpCount() << " Group(s)." << endl;
		cout << "# of Vertices: " << mPriModelPtr->getVertexCount() << " ("
				<< mPriModelPtr->getUniqueVertexCount() << " unique)." << endl;
		cout << "# of Normals: " << mPriModelPtr->getNormalCount() << " ("
				<< mPriModelPtr->getUniqueNormalCount() << " unique)." << endl;
		cout << "BoundingBox-Max: "
				<< mPriModelPtr->getBB()->getMax().toString() << endl;
		cout << "BoundingBox-Min: "
				<< mPriModelPtr->getBB()->getMin().toString() << endl;
		cout << "Variable-Content Vertices: " << mPriModelPtr->getVertexCount()
				<< endl;
		// TODO find out why he does not count the number of vertices correctly
		cout << "Variable-Content Triangles: " << mPriModelPtr->getFaceCount()
				<< endl;
	}

	Model *tempModel = mPriModelPtr;
	tempModel->setColorTable(mCTable);
	// do the cleanup - to get the model again one has to reload the file from disk
	cleanup();
	return tempModel;
}

void
ObjModelLoader::firstPass()
{
	string lastType = "";

	fs::path my_path(mPriFName);
	fs::ifstream objFile;
	objFile.open(my_path, ios::in);

	char line[200];
	vector<string> tokens;
	while (objFile.getline(line, 200)) {
		tokens = splitSpace(string(line));
		if (tokens[0] == ("o")) {
			//++grpCount;
			lastType = "o";
		}
		else if (tokens[0] == ("g")) {
//			removeSpecialCharsFromName(tokens[1]);
			string longname("");
			for (unsigned int i = 1; i < tokens.size(); ++i) {
				longname.append(tokens[i]);
				longname.append("_");
			}
			longname.erase(longname.end() - 1);
			mPriModelPtr->addGrpPtr(new MetaGroup(longname));
			lastType = "g";
		}
		else if (tokens[0] == ("mtllib")) {
			string longname("");
			for (unsigned int i = 1; i < tokens.size(); ++i) {
				longname.append(tokens[i]);
				longname.append(" ");
			}
			longname.erase(longname.end() - 1);
			parseMtl(fs::path(my_path.parent_path() / longname));
			lastType = "mtllib";
		}
		else if (tokens[0] == ("v")) {
			V3f* v = new V3f(atof(tokens[1].c_str()), atof(tokens[2].c_str()),
					atof(tokens[3].c_str()));
			mPriModelPtr->addVPtr(v);
			lastType = "v";
		}
		else if (tokens[0] == ("vt")) {
			V3f* t = new V3f(atof(tokens[1].c_str()), atof(tokens[2].c_str()),
					atof(tokens[3].c_str()));
			mPriModelPtr->addTPtr(t);
			lastType = "vt";
		}
		else if (tokens[0] == ("vn")) {
			V3b *n = new V3b((char) 127.0f * atof(tokens[1].c_str()),
					(char) 127.0f * atof(tokens[2].c_str()), (char) 127.0f * atof(tokens[3].c_str()));
			mPriModelPtr->addNPtr(n);
			lastType = "vn";
		}
		else if (tokens[0] == ("f")) {
			lastType = "f";
		}

	}
	objFile.close();

	// if we have not read any groups in, create a default group
	if (mPriModelPtr->getGrpCount() < 1) {
		mPriModelPtr->addGrpPtr(new MetaGroup());
	}
}

void
ObjModelLoader::secondPass()
{
	string lastType = "";
	int grpId = -1;
	mPriModelPtr->setCurrentGrp(mPriModelPtr->getGrpStart()->first);

	fs::path my_path(mPriFName);
	fs::ifstream objFile;
	objFile.open(my_path, ios::out);
	char line[200];
	vector<string> tokens;
	while (objFile.getline(line, 200)) {
		tokens = splitSpace(string(line));
		if (tokens[0] == ("g")) {
//			removeSpecialCharsFromName(tokens[1]);
			++grpId;
			lastType = "g";
			string longname("");
			for (unsigned int i = 1; i < tokens.size(); ++i) {
				longname.append(tokens[i]);
				longname.append("_");
			}
			longname.erase(longname.end() - 1);

			mPriModelPtr->setCurrentGrp(longname);
			//cout << _model.getCurrentGrpPtr()->name << endl;
		}
		else if (tokens[0] == ("v")) {
			lastType = "v";
		}
		// moved material assignment to 2nd pass because it references a group.
		// But only at end of 1st pass we know our groups
		else if (tokens[0] == ("usemtl")) {
			//			cout << "found material reference " << tokens[1] << " in obj-file" << endl;
			string longName("");
			for (unsigned int i = 1; i < tokens.size(); ++i) {
				longName.append(tokens[i]);
				longName.append(" ");
			}
			longName.erase(longName.end() - 1);
			if (mPriMatMap.find(longName) != mPriMatMap.end())
				mPriModelPtr->getCurrentGrpPtr()->setMat(*mPriMatMap.at(longName));
			lastType = "usemtl";
		}
		else if (tokens[0] == ("f")) {
			//cout << "Number of Components per face: " << tokens.size()-1 << endl;
			Face* f = new Face();
			f->norm = false;
			f->vert = false;
			f->matIdx = 0;
			f->tex = 0;
			f->fNormal = 0;

			string::size_type loc = tokens[1].find("/", 0);
			if (loc != string::npos) {
				for (string::size_type i = 1; i < tokens.size(); ++i) {
					vector<int> comp = extractFaceComponents(tokens[i]);
					// vertices
					if ((comp[0] & 4)) {
						V3f* vtx = mPriModelPtr->getVPtr(comp[1] - 1);
						f->vertexPtrList.push_back(vtx);
						vtx->addFaceRef(f);
						f->vert = true;
						mPriModelPtr->getCurrentGrpPtr()->nVertices++;
						mPriModelPtr->incVCount();
						mPriModelPtr->getCurrentGrpPtr()->bb->expand(*vtx);
					}
					// textures
					if ((comp[0] & 2)) {
						f->texturePtrList.push_back(mPriModelPtr->getTPtr(
								comp[2] - 1));
						++f->tex;
						mPriModelPtr->getCurrentGrpPtr()->nTextureCoords++;
						mPriModelPtr->incTCount();
					}
					// normals
					if ((comp[0] & 1)) {
						f->normalPtrList.push_back(mPriModelPtr->getNPtr(
								comp[3] - 1));
						f->norm = true;
						mPriModelPtr->getCurrentGrpPtr()->nNormals++;
						mPriModelPtr->incNCount();
					}
					comp.clear();
				}
			}
			else {
				V3f* vtx = mPriModelPtr->getVPtr(atoi(tokens[1].c_str()) - 1);
				f->vertexPtrList.push_back(vtx);
				mPriModelPtr->getCurrentGrpPtr()->bb->expand(*vtx);
				vtx = mPriModelPtr->getVPtr(atoi(tokens[2].c_str()) - 1);
				f->vertexPtrList.push_back(vtx);
				mPriModelPtr->getCurrentGrpPtr()->bb->expand(*vtx);
				vtx = mPriModelPtr->getVPtr(atoi(tokens[3].c_str()) - 1);
				f->vertexPtrList.push_back(vtx);
				mPriModelPtr->getCurrentGrpPtr()->bb->expand(*vtx);
				f->vert = true;
				mPriModelPtr->getCurrentGrpPtr()->nVertices += 3;
				mPriModelPtr->incVCount(3);

			}
			mPriModelPtr->addFPtrToCurrent(f);
			lastType = "f";
		}
	}
	objFile.close();
}

void
ObjModelLoader::parseMtl(fs::path file)
{
	//TODO parse Mtl-File and put the materials into the (hash)map
	// within the model.
	// remember: 1 object = 1 material = 1 vbo
	// mööööp  -wrong!
	if (!exists(file))
		cerr << "Material-File '" << file.string() << "' does not exist!"
				<< endl;

	Material* matPtr = 0;
	fs::ifstream mtlFile;
	mtlFile.open(file, ios::in);

	char line[200];
	vector<string> tokens;
	while (mtlFile.getline(line, 200)) {
		tokens = splitSpace(string(line));
		if (tokens[0] == ("newmtl")) {
			string longName("");
			for (unsigned int i = 1; i < tokens.size(); ++i) {
				longName.append(tokens[i]);
				longName.append(" ");
			}
			longName.erase(longName.end() - 1);
			matPtr = new Material();
			matPtr->kdR = ooctools::defaultColorF.getX();
			matPtr->kdG = ooctools::defaultColorF.getY();
			matPtr->kdB = ooctools::defaultColorF.getZ();
			mPriMatMap.insert(make_pair(longName, matPtr));
			//			cout << "found MaterialDefinition '" << tokens[1] << "'!" << endl;
		}
		else if (!strcasecmp("kd", tokens[0].c_str())) {
			matPtr->kdR = atof(tokens[1].c_str());
			matPtr->kdG = atof(tokens[2].c_str());
			matPtr->kdB = atof(tokens[3].c_str());
		}
	}
	mtlFile.close();
}

void
ObjModelLoader::setColorTable(const ColorTable& _ct)
{
	mCTable = _ct;
}

void
ObjModelLoader::cleanup()
{
	if (!mPriMatMap.empty()) {
		map<string, Material*>::iterator it = mPriMatMap.begin();
		for(; it!=mPriMatMap.end(); ++it){
			delete (it->second);
		}
		mPriMatMap.clear();
	}
	mPriModelPtr = 0;
	mPriFName.clear();
}

void
ObjModelLoader::debug()
{
	string s("287W4153-25>15/16\" GROUND>BACS12GU5K14>BACS12GU5K14_Geometry_0");
//	removeSpecialCharsFromName(s);
//	cout << "string became: " << s << endl;
}

} // oocformats
