/**
 * @file	MTLGenerator.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 10.03.2009
 *
 */

#include "MTLGenerator.h"

#include <iostream>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstddef>
#include <sstream>

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "StructDefs.h"
#include "ColorTable.h"
#include "../../OOCTools/include/declarations.h"
#include "../../OOCFormats/include/declarations.h"

using namespace std;
using namespace ooctools;
using namespace oocformats;
namespace fs = boost::filesystem;

namespace MTLGenerator {

MTLGenerator::MTLGenerator() {
	// TODO Auto-generated constructor stub

}

MTLGenerator::MTLGenerator(fs::path oP, fs::path xP, fs::path mP) : writtenFileCount(0), missMatches(0){
	mPrivObjFolder = oP;
	mPrivXlsFolder = xP;
	mPrivMtlFolder = mP;
}

MTLGenerator::~MTLGenerator() {
	// TODO Auto-generated destructor stub
}

bool MTLGenerator::find_file(const fs::path & dir_path, const string & file_name, fs::path & path_found )
{
	if ( !exists( dir_path ) ) return false;
	fs::directory_iterator end_itr; // default construction yields past-the-end
	for ( fs::directory_iterator itr( dir_path ); itr != end_itr; ++itr )
	{
		if ( fs::is_directory(itr->status()) )
		{
			if ( find_file( itr->path(), file_name, path_found ) ) return true;
		}
		else if ( itr->leaf() == file_name ) // see below
		{
			path_found = itr->path();
			return true;
		}
	}
	return false;
}

void MTLGenerator::matchObjsVsMtls(){
	fs::directory_iterator dir_iter(mPrivObjFolder), dir_end;
		for(;dir_iter != dir_end; ++dir_iter){
			if ( !is_directory(dir_iter->status()) && fs::extension(dir_iter->path())==".obj"){
				fs::path mtlFile( dir_iter->path());
				mtlFile.replace_extension(".mtl");
//				cout << mtlFile.filename() << endl;
//				exit(0);
				mtlFile = fs::path(mPrivMtlFolder / mtlFile.filename());


				adjustOBJ(mtlFile);



				if (!fs::exists(mtlFile)) {
					cerr << "The file " << (dir_iter->path()) << " has no matching Material-File "<< mtlFile << "!" << endl;
					cout << "generating mtl-file with default color " <<
					defaultColorF->getX()	<< ", " << defaultColorF->getY() <<
					", "<< defaultColorF->getZ() << "!" << endl;
					generateMissingMTL(mtlFile);
//					adjustOBJ(mtlFile);
					++missMatches;
				}
				else
				 ++writtenFileCount;
//					cout << mtlFile << "does exist!" << endl;
			}
		}
		cout << "total missmatches: " << missMatches<< endl;
		cout << "total hits: " << writtenFileCount<< endl;
		cout << "total files: " << missMatches+writtenFileCount << endl;
}

void MTLGenerator::matchMtlsVsObjs(){
	fs::directory_iterator dir_iter(mPrivMtlFolder), dir_end;
		for(;dir_iter != dir_end; ++dir_iter){
			if ( !is_directory(dir_iter->status()) && fs::extension(dir_iter->path())==".mtl"){
				fs::path objFile( dir_iter->path());
				objFile.replace_extension(".obj");
//				cout << mtlFile.filename() << endl;
//				exit(0);
				objFile = fs::path(mPrivObjFolder / objFile.filename());
				if (!fs::exists(objFile)) {
					cerr << "The file " << (dir_iter->path()) << " has no matching Obj-File "<< objFile << "!" << endl;
					++missMatches;
				}
				else
				 ++writtenFileCount;
//					cout << mtlFile << "does exist!" << endl;
			}
		}
		cout << "total missmatches: " << missMatches<< endl;
		cout << "total hits: " << writtenFileCount<< endl;
		cout << "total files: " << missMatches+writtenFileCount << endl;
}

void MTLGenerator::parseXLS(fs::path file){
	cout << "now parsing file '" << file.string() << "'......" << endl;
	int lineCount = 0;
	MatIterator matIt;
	ObjIterator objIt;
	Material *mat;
	MtlMap *obj;
//	if (exists(file)) cerr << file.string() << " existiert bereits!" << endl;
	char line[400];
	vector<string> tokens;
	fs::ifstream fb;
	fb.open( file, ios::in);
	while (fb.getline(line, 400)){
		++lineCount;
		tokens = splitTab(string(line));
//		if (tokens[0] == "DPA-E149W6000M16-DD-15.5"){
//			exit(0);
//		}
		if (tokens[0]!="Model ID") {// exclude header

			objIt = mPrivObjectMap.find(tokens[0]);
			if (objIt == mPrivObjectMap.end()) { // does this already exist?
				obj = new MtlMap();
				mPrivObjectMap.insert(make_pair(tokens[0], obj));
			}
			else {
				obj = objIt->second;
			}

			matIt = obj->matMap.find(tokens[1]);
			if (matIt == obj->matMap.end()){
				mat = new Material();
				obj->matMap.insert(make_pair(tokens[1], mat));
			}
			else{
				mat = matIt->second;
			}
			float quotient = 1.0f/255.0f;
			mat->kdR = quotient*atoi(tokens[2].c_str());
			mat->kdG = quotient*atoi(tokens[3].c_str());
			mat->kdB = quotient*atoi(tokens[4].c_str());

			// collecting the color-map
			ct.addColori(new V3ub((unsigned char)atoi(tokens[2].c_str()), (unsigned char)atoi(
					tokens[3].c_str()), (unsigned char)atoi(tokens[4].c_str())));
			// color-counting
			stringstream ss;
			ss << tokens[2] << "-" << tokens[3] << "-" << tokens[4];
			if (mPrivColorMultiverse.find(ss.str()) == mPrivColorMultiverse.end()){
				mPrivColorMultiverse.insert(make_pair(ss.str(),0));
			}
//			cout << tokens[0] << "@" << tokens[1] << "@" << mat->kdR << "@" << mat->kdG << "@" << mat->kdB << endl;

		}
	}
	fb.close();
	cout << "Done parsing file '" << file.string() << "'!" << endl;
	cout << "created " << mPrivObjectMap.size() << " Material-Objects from it!" << endl;
	cout << "lines parsed: " << lineCount << endl;
	cout << "---------------------------------------------------" << endl;
}

bool MTLGenerator::tab(char c){
	if (c == '\t') return true;
	else return false;
}

bool MTLGenerator::no_tab(char c){
	if (c != '\t') return true;
	else return false;
}

bool MTLGenerator::space(char c){
	return isspace(c);
}

bool MTLGenerator::no_space(char c){
	return !isspace(c);
}

vector<string> MTLGenerator::splitTab(const string& str){
	typedef string::const_iterator iter;
	vector<string> result;
	iter i = str.begin();
	while (i!=str.end()){
		i = find_if(i, str.end(), MTLGenerator::no_tab);
		iter j = find_if(i, str.end(), MTLGenerator::tab);
		if (i != str.end())
			result.push_back(string(i,j));
		i = j;
	}
	return result;
}

void createMatFromLine(std::string){

}

void MTLGenerator::writeCollectionToMtl(fs::path colorCollectionFile){
	cout << "writing MTLs from color-file " << colorCollectionFile << " ........."<< endl;
	for(ObjIterator objIt=mPrivObjectMap.begin(); objIt!=mPrivObjectMap.end(); ++objIt){

		if (fs::exists(mPrivObjFolder / string(objIt->first + ".obj")))
			writeMtl(objIt);
//		adjustOBJ(objIt);
	}
	cout << "Done!"<< endl;
}

void MTLGenerator::writeMtl(ObjIterator objPtr){
//	cout << "writing " << objPtr->first << endl;
	if(exists(fs::path(mPrivMtlFolder / string(objPtr->first + ".mtl")))){
		// merge with existing
		// yeah...it's not really efficient but we only have like 9 files
		cerr << "Datei " << string(objPtr->first + ".mtl") << " existiert bereits! - exiting!" << endl;
		exit(0);
		//parseMTL(fs::path(mPrivDstFolder / string(objPtr->first + ".mtl")), objPtr);
	}
	else {
//		cout << fs::path(mPrivDstFolder / string(objPtr->first + ".mtl")) << " gübtsnocnnüch-writing" << endl;
		fs::ofstream of;
		of.open( fs::path(mPrivMtlFolder / string(objPtr->first + ".mtl")), ios::out);
		for(MatIterator matIt = objPtr->second->matMap.begin(); matIt!=objPtr->second->matMap.end(); ++matIt){
//			cout << "matname: " << matIt->first << endl;
			of << "newmtl " << matIt->first << endl;
			of << "Kd " << matIt->second->kdR << " " << matIt->second-> kdG << " " << matIt->second->kdB << endl;
		}
		of << endl;
		of.close();
		++writtenFileCount;
	}

}

void MTLGenerator::doTheParsing(){
	fs::directory_iterator dir_iter(mPrivXlsFolder), dir_end;
	for(;dir_iter != dir_end; ++dir_iter){
		if ( !is_directory(dir_iter->status()) && fs::extension(dir_iter->path())==".xls"){
			parseXLS(dir_iter->path());
//			writeCollectionToMtl(dir_iter->path());
			ct.writeToFile(fs::path(mPrivXlsFolder / "colortable.bin"));
			cleanup();
		}
	}
	cout << "All done! We have written " << writtenFileCount << " files to '" << mPrivMtlFolder << "'!" << endl;
	cout << "Now for the sad part - we have that many different colors: " << mPrivColorMultiverse.size() << endl;

}

void MTLGenerator::cleanup(){
	for (ObjIterator objIt=mPrivObjectMap.begin(); objIt!=mPrivObjectMap.end(); ++objIt){
		for (MatIterator matIt = objIt->second->matMap.begin(); matIt!=objIt->second->matMap.end(); ++matIt){
			delete matIt->second;
		}
		objIt->second->matMap.clear();
		delete objIt->second;
	}
	mPrivObjectMap.clear();
}

void MTLGenerator::parseMTL(fs::path p, ObjIterator objIt){
	Material *mat=NULL;
	MtlMap *obj;
	char line[200];
	vector<string> tokens;
	fs::ifstream fb;
	fb.open( p, ios::in);
	while (fb.getline(line, 200)){
		tokens = splitSpace(string(line));
		if (tokens[0]=="newmtl") {
			mat = new Material();
			objIt->second->matMap[tokens[1]]=mat;






			objIt = mPrivObjectMap.find(tokens[0]);
			if (objIt == mPrivObjectMap.end()) { // does this already exist?
				obj = new MtlMap();
				mPrivObjectMap.insert(make_pair(tokens[0], obj));
			}
			else {
				obj = objIt->second;
			}
			mat->kdR = 255.0f / atoi(tokens[2].c_str());
			mat->kdG = 255.0f / atoi(tokens[3].c_str());
			mat->kdB = 255.0f / atoi(tokens[4].c_str());
			cout << tokens[0] << "@" << tokens[1] << "@" << tokens[2] << "@" << tokens[3] << "@" << tokens[4] << endl;
		}
	}
	fb.close();
	cout << "we have " << mPrivObjectMap.size() << endl;
}

vector<string> MTLGenerator::splitSpace(const string& str){
	typedef string::const_iterator iter;
	vector<string> result;
	iter i = str.begin();
	while (i!=str.end()){
		i = find_if(i, str.end(), MTLGenerator::no_space);
		iter j = find_if(i, str.end(), MTLGenerator::space);
		if (i != str.end())
			result.push_back(string(i,j));
		i = j;
	}
	return result;
}

void MTLGenerator::adjustOBJ(ObjIterator objPtr){
	string fName(objPtr->first);
	fName.append(".obj");
	string newFName = fName;
	newFName.append("_temp");
	if (exists(mPrivObjFolder / fName)){
		// TODO zum einen jenes obj-file durchparsen
		string line;
		vector<string> tokens;
		fs::ifstream inFile;
		fs::ofstream outFile;
		inFile.open( mPrivObjFolder / fName, ios::in);
		outFile.open( mPrivObjFolder / newFName, ios::out);
		// first of all write the mtllib to the new file
		outFile << "mtllib " << objPtr->first << ".mtl"<< endl;
		while (!inFile.eof()){
			getline(inFile, line);
			if (line.substr(0, 1) == "g"){
				// copy this line and extract group-name
				// then insert an usemtl line right after that one
				outFile << line << endl;
				outFile << "usemtl " << line.substr(2) << endl;
			}
			else if (line.substr(0, 6) == "usemtl"){
				// ignore a.k.a. don't copy
			}
			else if (line.substr(0, 6) == "mtllib"){
				// ignore a.k.a. don't copy
			}
			else if(line.substr(0, 2) == "v " || line.substr(0, 3) == "vn " || line.substr(0, 2) == "f " ||
					line.substr(0, 3) == "vt " || line.substr(0, 2) == "o " || line.substr(0, 1) == "#" ||
					line.substr(0, 3) == "vp "){
				outFile << line << endl;
				// just copy
			}
		}
		inFile.close();
		outFile.close();
		//TODO Kosmetik: first delete then rename - it's cheaper than copy-delete
		fs::remove(mPrivObjFolder / fName);
		fs::rename(mPrivObjFolder / newFName, mPrivObjFolder / fName);
//		fs::copy_file(mPrivObjFolder / newFName, mPrivObjFolder / fName);
//		fs::remove(mPrivObjFolder / newFName);

		// zum anderen eine veränderte copy speichern und am ende die eine in die andere moven
//		cout << objPtr->first << endl;
	}
	else {
		cout << "No Object-File '" << fName << "' found. - skipping!" << endl;
	}
//	exit(0);
}

void MTLGenerator::adjustOBJ(fs::path &mtlFile){
	fs::path fName(mtlFile.filename());
	fName.replace_extension(".obj");
	string newFName = fName.string();
	newFName.append("_temp");
	if (exists(mPrivObjFolder / fName)){
		// TODO zum einen jenes obj-file durchparsen
		string line;
		fs::ifstream inFile;
		fs::ofstream outFile;
		inFile.open( mPrivObjFolder / fName, ios::in);
		outFile.open( mPrivObjFolder / newFName, ios::out);
		// first of all write the mtllib to the new file
		outFile << "mtllib " << mtlFile.filename() << endl;
		while (!inFile.eof()){
			getline(inFile, line);
			if (line.substr(0, 2) == "g "){
				// copy this line and extract group-name
				// then insert an usemtl line right after that one
				outFile << line << endl;
				outFile << "usemtl " << line.substr(2) << endl;
			}
			else if (line.substr(0, 6) == "usemtl"){
				// ignore a.k.a. don't copy
			}
			else if (line.substr(0, 6) == "mtllib"){
				// ignore a.k.a. don't copy
			}
			else if(line.substr(0, 2) == "v " || line.substr(0, 3) == "vn " || line.substr(0, 2) == "f " ||
					line.substr(0, 3) == "vt " || line.substr(0, 2) == "o " || line.substr(0, 1) == "#" ||
					line.substr(0, 3) == "vp "){
				outFile << line << endl;
				// just copy - !BUT! remove everything else
			}
		}
		inFile.close();
		outFile.close();
		//TODO Kosmetik: first delte then rename - it's cheaper than copy-delete
		fs::remove(mPrivObjFolder / fName);
		fs::rename(mPrivObjFolder / newFName, mPrivObjFolder / fName);
//		fs::copy_file(mPrivObjFolder / newFName, mPrivObjFolder / fName);
//		fs::remove(mPrivObjFolder / newFName);

		// zum anderen eine veränderte copy speichern und am ende die eine in die andere moven
//		cout << objPtr->first << endl;
	}
	else {
		cout << "No Object-File '" << fName << "' found. - skipping!" << endl;
	}
//	exit(0);
}

void MTLGenerator::generateMissingMTL(fs::path &mtlFile){
	// parse relating OBJ and save grp-names as mtlname in mtl-file
	fs::path objFile(mPrivObjFolder / mtlFile.filename());
	objFile.replace_extension(".obj");
	string line;
	fs::ifstream inFile;
	fs::ofstream outFile;
	inFile.open( objFile, ios::in);
	outFile.open( mtlFile, ios::out);
	while (!inFile.eof()){
		getline(inFile, line);
		if (line.substr(0,1)=="g"){
//			cout << "writing " << mtlFile << endl;
			outFile << "newmtl " << line.substr(2) << endl;
			outFile << "Kd " << defaultColorF->getX() << " " <<
			defaultColorF->getY() << " " << defaultColorF->getZ() << endl;
		}
	}
	outFile << endl;
	inFile.close();
	outFile.close();
}

void MTLGenerator::flipOAndG(){
	fs::directory_iterator dir_iter(mPrivObjFolder), dir_end;
		for(;dir_iter != dir_end; ++dir_iter){
			if ( !is_directory(dir_iter->status()) && fs::extension(dir_iter->path())==".obj"){
				// open it and open temp
				string line;
				string tempFileName(dir_iter->path().filename());
				tempFileName.append("_temp");
				fs::ifstream inFile;
				fs::ofstream outFile;
				inFile.open( mPrivObjFolder / dir_iter->path().filename(), ios::in);
				outFile.open( mPrivObjFolder / tempFileName, ios::out);
				while (!inFile.eof()){
					getline(inFile, line);
					if (line.substr(0, 2) == "o "){
						outFile << "g " << line.substr(2) << endl;
					}
					else if (line.substr(0, 2) == "g "){
						outFile << "o " << line.substr(2) << endl;
					}
					else{
						outFile << line << endl;
					}
				}
				outFile << endl;
				inFile.close();
				outFile.close();
				fs::remove(mPrivObjFolder / dir_iter->path().filename());
				fs::rename(mPrivObjFolder / tempFileName, mPrivObjFolder / dir_iter->path().filename());
			}
		}

}

void MTLGenerator::setDirs(fs::path _objDir, fs::path _xlsDir, fs::path _mtlDir){
	mPrivObjFolder = _objDir;
	mPrivMtlFolder = _mtlDir;
	mPrivXlsFolder = _xlsDir;
}

void
MTLGenerator::writeColorTable(fs::path _path){

}

void MTLGenerator::debug(){
	parseXLS(mPrivXlsFolder / "CD#2.colors.xls");
//	writeMtl(mPrivObjectMap.begin());
	writeCollectionToMtl(mPrivXlsFolder / "CD#2.colors.xls");
}

} // end of namespace MTLGenerator
