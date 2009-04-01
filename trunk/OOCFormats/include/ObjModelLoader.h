/*
 * ObjModelLoader.h
 *
 *  Created on: 02.02.2009
 *      Author: ava
 */

#ifndef OBJMODELLOADER_H_
#define OBJMODELLOADER_H_

#include <vector>
#include <string>
#include <map>

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "../../OOCTools/include/declarations.h"
#include "declarations.h"

namespace fs = boost::filesystem;
namespace oocformats {

class ObjModelLoader {
public:
	ObjModelLoader();
	ObjModelLoader(ooctools::ColorTable* ct);
	virtual ~ObjModelLoader();

	// this is THE method to call......
	ooctools::Model *parseMultipass(std::string _fname, bool verbose= false);
	void setColorTable(ooctools::ColorTable* _ct);

private:
	// this is just a temporary store. after parsing this reference will be nullified
	// thus the receiver of praseMultipass() is responsible for destruction.
	ooctools::Model *mPriModelPtr;
	std::string mPriFName;
	ooctools::ColorTable* mCTable;

	std::map<std::string, ooctools::Material*> mPriMatMap;

	std::vector<std::string> splitSpace(const std::string& str);
	std::vector<std::string> splitSlash(const std::string& str);

	// return a bitvector to find out which components of a face are in the model (vertex, normal, texCoords)
	std::vector<int> extractFaceComponents(const std::string& s);
	void firstPass();
	void secondPass();
	void parseMtl(fs::path file);
	void cleanup();

	static bool space(char c);
	static bool no_space(char c);
	static bool slash(char c);
	static bool no_slash(char c);

	static ooctools::V3f mPriDefaultVertex;
	static ooctools::MetaGroup mPriDefaultGrp;

};

} // oocformats

#endif /* OBJMODELLOADER_H_ */
