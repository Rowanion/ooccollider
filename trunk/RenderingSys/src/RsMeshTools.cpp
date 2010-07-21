/**
 * @file	RsMeshTools.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 28.09.2009
 * @brief 	RsMeshTools class definition.
 */


#include "RsMeshTools.h"
#include "RsImageTools.h"

#include <iostream>

#include "GL/glew.h"

#include "RsStructs.h"
#include "RsVectorMath.h"

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

void RsMeshTools::parseObj(fs::path* _file, RsObjInfo* _info)
{
	if (_info == 0)
		return;

	std::string line;
	const boost::regex usemtl_expr("usemtl");
	std::pair<std::map<std::string, unsigned>::iterator, bool > result;
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> blank_sep(" ");
	tokenizer tokens = tokenizer(line, blank_sep);
	tokenizer face_tokens = tokenizer(line, blank_sep);
	tokenizer::iterator tok_iter;

	fs::ifstream inFile;
	inFile.open(*_file);
	while (std::getline(inFile, line)) {
		tokens.assign(line, blank_sep);
		if (tokens.begin()!= tokens.end()){
			string type = (*tokens.begin());
			switch (type[0]){
			case 'u':	// Probable point of mtllib
				if (boost::regex_match(type, usemtl_expr, boost::match_extra)){
					tok_iter = tokens.begin();
					tok_iter++;
					result = _info->materialNames.insert(make_pair(std::string(*tok_iter), _info->materialCount));
					if (result.second){	//new entry inserted
						_info->materialCount++;
					}
				}
				break;
			case 'g':	// Probable point of group
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
void RsMeshTools::loadMtlLib(fs::path* _file, RsObjInfo* _info)
{
	std::string line;
	boost::char_separator<char> blank_sep(" \t");
	tokenizer tokens = tokenizer(line, blank_sep);
	tokenizer face_tokens = tokenizer(line, blank_sep);
	const boost::regex newmtl_expr("newmtl");
	const boost::regex mtlka_expr("Ka");
	const boost::regex mtlkd_expr("Kd");
	const boost::regex mtlks_expr("Ks");
	const boost::regex mtlns_expr("Ns");
	const boost::regex mtlmapkd_expr("map_Kd");
	const boost::regex mComponents_expr("(\\d+)/?(\\d*)/?(\\d*)");
	tokenizer::iterator tok_iter;
	RsMaterial* matPtr = 0;
	std::pair<std::set<RsMaterial>::iterator, bool > setResult;
	std::map<std::string, unsigned>::iterator mapIter;

	fs::ifstream inFile;
	inFile.open(*_file);
	while (std::getline(inFile, line)) {
		tokens.assign(line, blank_sep);
		if (tokens.begin()!= tokens.end()){
			tok_iter = tokens.begin();
			string type = (*tok_iter);
			if (boost::regex_match(type, newmtl_expr, boost::match_extra)){ // found new material
				tok_iter++;
				mapIter = _info->materialNames.find(*tok_iter);
				if (mapIter != _info->materialNames.end()){
					matPtr = new RsMaterial();
					matPtr->id = mapIter->second;
					matPtr->name = *tok_iter;
					setResult = _info->material.insert(*matPtr);
					delete matPtr;
					matPtr = 0;
					matPtr = (RsMaterial*)(&(*setResult.first));
				}
				else { // ignore unreferenced material
					matPtr = 0;
				}
			}
			else if (boost::regex_match(type, mtlka_expr, boost::match_extra)){ // found ambient value
				if (matPtr != 0){
					tok_iter++;
					matPtr->ambient.x = atof(tok_iter->c_str());
					tok_iter++;
					matPtr->ambient.y = atof(tok_iter->c_str());
					tok_iter++;
					matPtr->ambient.z = atof(tok_iter->c_str());
				}
			}
			else if (boost::regex_match(type, mtlkd_expr, boost::match_extra)){ // found diffuse value
				if (matPtr != 0){
					tok_iter++;
					matPtr->diffuse.x = atof(tok_iter->c_str());
					tok_iter++;
					matPtr->diffuse.y = atof(tok_iter->c_str());
					tok_iter++;
					matPtr->diffuse.z = atof(tok_iter->c_str());
				}
			}
			else if (boost::regex_match(type, mtlks_expr, boost::match_extra)){ // found specular value
				if (matPtr != 0){
					tok_iter++;
					matPtr->specular.x = atof(tok_iter->c_str());
					tok_iter++;
					matPtr->specular.y = atof(tok_iter->c_str());
					tok_iter++;
					matPtr->specular.z = atof(tok_iter->c_str());
				}
			}
			else if (boost::regex_match(type, mtlns_expr, boost::match_extra)){ // found shininess value
				if (matPtr != 0){
					tok_iter++;
					matPtr->shininess = atof(tok_iter->c_str());
				}
			}
			else if (boost::regex_match(type, mtlmapkd_expr, boost::match_extra)){ // found diffuse map
				if (matPtr != 0){
					tok_iter++;
					mapIter = _info->textureNames.find(*tok_iter);
					if (mapIter == _info->textureNames.end()){
						GLuint texId;
						glGenTextures(1, &texId);
						_info->textureNames.insert(make_pair(*tok_iter, texId));
						matPtr->texture = texId;
						_info->texCount++;
					}
				}
			}
		}
	}
	if (_info->texCount>0){
		boost::filesystem::path fullPath = _file->branch_path() / "images";
		this->loadTextures(&fullPath, _info);
	}
}

RsObjModel* RsMeshTools::loadObj(fs::path* _file)
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

	if (!boost::filesystem::is_regular_file(*_file)){
		return 0;
	}

	// first pass
	RsObjInfo modelInfo = RsObjInfo();
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
	RsObjModel* model = new RsObjModel(&modelInfo);
//	exit(0);

	// generate temporary space for model
	float* vertices = new float[modelInfo.vertexCount*3];
	unsigned vComponentCount = 0;
	char* normals = 0;
	unsigned nComponentCount = 0;
	float* texCoords = 0;
	unsigned tComponentCount = 0;
	unsigned char* colors = 0;
	unsigned currentMaterial = 0;
	unsigned* groupMaterial = new unsigned[modelInfo.groupCount];
	for (unsigned i =0; i< modelInfo.groupCount; ++i){
		groupMaterial[i] = 0;
	}

	unsigned** group = new unsigned*[modelInfo.groupCount];
	for (unsigned i = 0; i < modelInfo.groupCount; ++i){
		if (modelInfo.groupBits[i]>2){
			group[i] = new unsigned[modelInfo.groupFaces[i]*3*3];
		}
		else if (modelInfo.groupBits[i]>0){
			group[i] = new unsigned[modelInfo.groupFaces[i]*2*3];
		}
		else{
			group[i] = new unsigned[modelInfo.groupFaces[i]*3];
		}
	}
	unsigned groupIndexComponentCount = 0;
	int gCount = -1;
	if (modelInfo.normalCount>0){
		normals = new char[modelInfo.normalCount*3];
	}
	if (modelInfo.texCount>0){
		texCoords = new float[modelInfo.texCount*2];
	}
	if (modelInfo.materialCount>0){
		colors = new unsigned char[modelInfo.materialCount*4];
	}


	// second pass - actual loading
	std::string line;
	boost::char_separator<char> blank_sep(" \t");
	tokenizer tokens = tokenizer(line, blank_sep);
	tokenizer face_tokens = tokenizer(line, blank_sep);
	const boost::regex usemtl_expr("usemtl");
	const boost::regex mtllib_expr("mtllib");
	const boost::regex mComponents_expr("(\\d+)/?(\\d*)/?(\\d*)");
	std::map<std::string, unsigned>::iterator mapIter;

	fs::ifstream inFile;
	inFile.open(*_file);
	while (std::getline(inFile, line)) {
		tokens.assign(line, blank_sep);
		if (tokens.begin()!= tokens.end()){
			string type = (*tokens.begin());
			switch (type[0]){
			case 'm':	// Probable point of mtllib
				if (boost::regex_match(type, mtllib_expr, boost::match_extra)){
					// -------------------------------------------------------
					tokenizer::iterator tok_iter=tokens.begin();
					tok_iter++;
					boost::filesystem::path mtlP = boost::filesystem::path(_file->branch_path() / *tok_iter);
					if (boost::filesystem::is_regular_file(mtlP)){
						this->loadMtlLib(&mtlP, &modelInfo);
					}

					// --------------------------------------------------------
				}
				break;
			case 'u':	// Probable point of usemtl
				if (boost::regex_match(type, usemtl_expr, boost::match_extra)){
					tokenizer::iterator tok_iter=tokens.begin();
					tok_iter++;
					mapIter = modelInfo.materialNames.find(*tok_iter);
					if (mapIter != modelInfo.materialNames.end()){
						currentMaterial = mapIter->second;
					}

				}
				break;
			case 'g':	// Probable point of group
				gCount++;
				groupIndexComponentCount = 0;
				break;
			case 'v':{
				if (type.size() == 1){ // Probable point of vertex-entry
					//TODO
					tokenizer::iterator tok_iter=tokens.begin();
					tok_iter++;
					for(; tok_iter!=tokens.end();++tok_iter){
						vertices[vComponentCount] = atof(tok_iter->c_str());
						vComponentCount++;
					}
				}
				else if (type[1]=='t'){ // Probable point of texture-entry
					//TODO
					tokenizer::iterator tok_iter=tokens.begin();
					tok_iter++;
					for(; tok_iter!=tokens.end();++tok_iter){
						texCoords[tComponentCount] = atof(tok_iter->c_str());
						tComponentCount++;
					}
				}
				else if (type[1]=='n'){ // Probable point of normal-entry
					//TODO
					tokenizer::iterator tok_iter=tokens.begin();
					tok_iter++;
					for(; tok_iter!=tokens.end();++tok_iter){
						// adjust to signed char range: -128 to 127
						float comp = atof(tok_iter->c_str());
						if (comp <0){
							normals[nComponentCount] = (char)(comp*128.0);
						}
						else{
							normals[nComponentCount] = (char)(comp*127.0);
						}

						nComponentCount++;
					}
				}
				break;}
			case 'f':{	// Probable point of face-entry
				if (gCount<0){
					gCount = 0;
					groupIndexComponentCount = 0;
				}
				groupMaterial[gCount] = currentMaterial;
				for(tokenizer::iterator tok_iter=tokens.begin(); tok_iter!=tokens.end();++tok_iter){
//					std::cout << "<" << *tok_iter << "> ";
					boost::smatch what;
					if (boost::regex_match(*tok_iter, what, mComponents_expr, boost::match_extra)){
						for(unsigned i = 1; i < what.size(); ++i){
							if (what[i].str().size() != 0){
//								std::cout << "      $" << i << " = \"" << what[i] << "\"\n";
//								cerr << "" << endl;
								group[gCount][groupIndexComponentCount] = atoi(what[i].str().c_str())-1;
								groupIndexComponentCount++;
							}
						}
					}
				}
//				cerr << line << endl;
//				std::cout << "\n";
				break;}
			default:
				break;
			}
		}
	}
	inFile.close();
	model->setMaterials(&modelInfo);

	for (unsigned i=0; i< modelInfo.groupCount; ++i){
		model->addVbo(&modelInfo, i, group[i], vertices, normals, texCoords, colors, groupMaterial[i]);
	}

	delete[] vertices;
	delete[] normals;
	delete[] texCoords;
	delete[] colors;
	for (unsigned i = 0; i< modelInfo.groupCount; ++i){
		delete[] group[i];
	}
	delete group;
	return model;
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

void RsMeshTools::loadTextures(boost::filesystem::path* _path, RsObjInfo* _info)
{
	RsTGAimage img;
	RsImageTools* it = RsImageTools::getSingleton();
	std::map<std::string, GLuint>::iterator mapIt = _info->textureNames.begin();
	boost::filesystem::path fullPath;
	for (; mapIt != _info->textureNames.end(); ++mapIt){
		fullPath = (*_path) /  mapIt->first;
			it->loadTGA(&fullPath, &img);

			glBindTexture(GL_TEXTURE_2D, mapIt->second);
			glTexImage2D(GL_TEXTURE_2D, 0, img.intlFormat, img.width, img.height, 0, img.format, GL_UNSIGNED_BYTE, img.data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	}
}
