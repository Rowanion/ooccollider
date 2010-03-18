/*
 * RsMeshTools.cpp
 *
 *  Created on: 16.03.2010
 *      Author: TheAvatar
 */

#include "RsMeshTools.h"

#include <iostream>

#include <boost/system/config.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/tokenizer.hpp>
#include <boost/regex.hpp>


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
	 */
	std::string line;
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> blank_sep(" ");
	tokenizer tokens = tokenizer(line, blank_sep);
	tokenizer face_tokens = tokenizer(line, blank_sep);
	const boost::regex usemtl_expr("usemtl");
	const boost::regex mtllib_expr("mtllib");
	const boost::regex sComponent_expr("(\\d+)");
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
