/*
 * RsMeshTools.cpp
 *
 *  Created on: 16.03.2010
 *      Author: TheAvatar
 */

#include "RsMeshTools.h"

#include <iostream>


#include "RsStructs.h"

using namespace std;
namespace fs = boost::filesystem;

RsMeshTools* RsMeshTools::instance = 0;

RsMeshTools* RsMeshTools::getSingleton()
{
	static Guard g;
	if( instance == 0 ){
		instance = new RsMeshTools();
	}
	return instance;
}

RsMeshTools::Guard::~Guard()
{
	if( RsMeshTools::instance != 0 )
		delete RsMeshTools::instance;
}

RsMeshTools::RsMeshTools()
{
	// TODO Auto-generated constructor stub

}

RsMeshTools::~RsMeshTools()
{
	// TODO Auto-generated destructor stub
}

void RsMeshTools::parseObj(fs::path* _file, ObjInfo* _info)
{
	if (_info == 0)
		return;

	std::string line;
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> blank_sep(" ");
	tokenizer tokens = tokenizer(line, blank_sep);
	tokenizer face_tokens = tokenizer(line, blank_sep);

	fs::ifstream inFile;
	inFile.open(*_file);
	while (std::getline(inFile, line)) {
		tokens.assign(line, blank_sep);
		if (tokens.begin()!= tokens.end()){
			string type = (*tokens.begin());
			switch (type[0]){
			case 'u':	// Probable point of mtllib
				_info->materialCount++;
				break;
			case 'g':	// Probable point of mtllib
				_info->groupCount++;
				_info->groupFaces.push_back(0);
				_info->groupBits.push_back(0);
				break;
			case 'f':	// Probable point of mtllib
				_info->faceCount++;
				if (_info->groupCount == 0){
					_info->groupCount++;
					_info->groupFaces.push_back(0);
					_info->groupBits.push_back(0);
				}

				if (_info->groupFaces.back() == 0){
					_info->groupBits.back() = analyzeFaceLine(&tokens);
				}
				_info->groupFaces.back()++;
				break;
			case 'v':{
				if (type.size() == 1){ // Probable point of vertex-entry
					_info->vertexCount++;
				}
				else if (type[1]=='t'){ // Probable point of texture-entry
					_info->texCount++;
				}
				else if (type[1]=='n'){ // Probable point of normal-entry
					_info->normalCount++;
				}
				break;}
			}
		}
	}
	inFile.close();
}

void RsMeshTools::loadObj(fs::path* _file)
{
	/**
	 * Idea: make multiple expressions for each case of the face-formats
	 * test then on the occurence of the first face and set it to that...
	 * But maybe there are different formats for different groups/objects (look that up)
	 * This seems to be the most convenient way
	 * http://www.boost.org/doc/libs/1_42_0/libs/regex/doc/html/boost_regex/captures.html
	 * http://www.boost.org/doc/libs/1_42_0/libs/regex/doc/html/index.html
	 * case1: v1 v2 v3
	 * case2: v1//vn1
	 * case3: v1/vt1
	 * case4: v1/vt1/vn1
	 *
	 * Another important sidenote:
	 * If we want to cull within a single obj or object, we have to break groups apart,
	 * making it harder to animate the model, because no inter-group-connections are known.
	 * Thus each group gets its own vertexbuffer and indexbuffer.
	 * If we know for sure, that there is no culling or within a particular obj,
	 * we can have an indexbuffer for each group, sharing a vertexbuffer with all
	 * its fellow groups.
	 */

	// first pass
	ObjInfo modelInfo = ObjInfo();
	this->parseObj(_file, &modelInfo);
	std::cerr << "File :" << *_file << std::endl;
	std::cerr << "Vertices :" << modelInfo.vertexCount << std::endl;
	std::cerr << "Normals :" << modelInfo.normalCount << std::endl;
	std::cerr << "TexCoords :" << modelInfo.texCount << std::endl;
	std::cerr << "Faces :" << modelInfo.faceCount << std::endl;
	std::cerr << "Groups :" << modelInfo.groupCount << std::endl;
	for (unsigned i=0; i< modelInfo.groupCount; i++){
		std::cerr << "Group " << i << " has " << modelInfo.groupFaces[i] << " faces";
		if (modelInfo.groupBits[i] & 1){
			std::cerr << " and has Textures";
		}
		if (modelInfo.groupBits[i] & 2){
			std::cerr << " and has Normals";
		}
		std::cerr << "." << std::endl;
	}
	std::cerr << "Materials :" << modelInfo.materialCount << std::endl;
	exit(0);
	// second pass

	std::string line;
	boost::char_separator<char> blank_sep(" ");
	tokenizer tokens = tokenizer(line, blank_sep);
	tokenizer face_tokens = tokenizer(line, blank_sep);
	const boost::regex usemtl_expr("usemtl");
	const boost::regex mtllib_expr("mtllib");
	const boost::regex mComponents_expr("(\\d+)/?(\\d*)/?(\\d*)");

	fs::ifstream inFile;
	inFile.open(*_file);
	while (std::getline(inFile, line)) {
		tokens.assign(line, blank_sep);
		if (tokens.begin()!= tokens.end()){
			string type = (*tokens.begin());
			switch (type[0]){
			case 'm':	// Probable point of mtllib
				if (boost::regex_match(type, mtllib_expr, boost::match_extra)){

				}
				break;
			case 'u':	// Probable point of usemtl
				if (boost::regex_match(type, usemtl_expr, boost::match_extra)){

				}
				break;
			case 'v':{
				if (type.size() == 1){ // Probable point of vertex-entry
					//TODO
				}
				else if (type[1]=='t'){ // Probable point of texture-entry
					//TODO
				}
				else if (type[1]=='n'){ // Probable point of normal-entry
					//TODO
				}
//				for(tokenizer::iterator tok_iter=tokens.begin(); tok_iter!=tokens.end();++tok_iter){
//					std::cout << "<" << *tok_iter << "> ";
//				}
//				std::cout << "\n";
				break;}
			case 'f':{	// Probable point of face-entry
				for(tokenizer::iterator tok_iter=tokens.begin(); tok_iter!=tokens.end();++tok_iter){
					std::cout << "<" << *tok_iter << "> ";
					boost::smatch what;
					if (boost::regex_match(*tok_iter, what, mComponents_expr, boost::match_extra)){
						for(unsigned i = 0; i < what.size(); ++i)
							std::cout << "      $" << i << " = \"" << what[i] << "\"\n";
					}
				}
				cerr << line << endl;
				std::cout << "\n";
				break;}
			default:
				break;
			}
		}
	}
	inFile.close();
}

unsigned RsMeshTools::analyzeFaceLine(tokenizer* _tokens)
{
	// 01 => textures = true
	// 10 => normals = true
	unsigned bitField = 0;


	const boost::regex mComponents_expr("(\\d+)/?(\\d*)/?(\\d*)");

	for(tokenizer::iterator tok_iter=_tokens->begin(); tok_iter!=_tokens->end(); ++tok_iter){
		boost::smatch what;
		if (boost::regex_match(*tok_iter, what, mComponents_expr, boost::match_extra)){
			unsigned i, mult;
			for(mult = 1, i = 2; i < what.size(); ++i, mult *= 2){
//				std::cout << "      $" << i << " = \"" << what[i] << "\"\n";
				if (what[i].str().size() != 0){
					bitField |= mult;
				}
			}
			return bitField;
		}
	}
	return bitField;
}
