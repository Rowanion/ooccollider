/**
 * @file	MTLGenerator.h
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 10.03.2009
 * @brief 	MTLGenerator class declaration
 */

#ifndef MTLGENERATOR_H_
#define MTLGENERATOR_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "StructDefs.h"
#include "ColorTable.h"
#include "BoundingBox.h"
#include "../../OOCTools/include/declarations.h"
#include "../../OOCFormats/include/declarations.h"

namespace fs = boost::filesystem;
namespace MTLGenerator {

struct SceneHeader{
	ooctools::BoundingBox bb;
	unsigned long long triCount;
	unsigned long long vertexCount;
};


/**
 * @class MTLGenerator
 * @brief A tool-collection to parse anything scene-related.
 * First i only intended to parse some xls/csv files which contained model-related
 * color-information. But over time it grew to a parsing collection - everything related to
 * preprocessing, like removal of erroneous chars from the model/mtl files, check for correctness
 * of a obj-mtl pair, etc.
 */
class MTLGenerator {
	typedef std::map<std::string, ooctools::MtlMap*>::iterator ObjIterator;
	typedef std::map<std::string, ooctools::Material*>::iterator MatIterator;

public:
	MTLGenerator();
	MTLGenerator(fs::path oP, fs::path xP, fs::path mP);


	virtual ~MTLGenerator();
	bool find_file(const fs::path & dir_path, const std::string & file_name, fs::path & path_found );
	void parseXLS(fs::path file);
	void createMatFromLine(std::string);
	void matchMtlsVsObjs();
	void matchObjsVsMtls();
	void writeCollectionToMtl(fs::path colorCollectionFile);
	std::vector<std::string> splitTab(const std::string& str);
	std::vector<std::string> splitSpace(const std::string& str);
	static bool tab(char c);
	static bool no_tab(char c);
	static bool space(char c);
	static bool no_space(char c);
	void writeMtl(ObjIterator objIt);
	void adjustOBJ(ObjIterator objIt);
	void adjustOBJ(fs::path &mtlFile);
	void parseMTL(fs::path p, ObjIterator objIt);
	void doTheParsing();
	void cleanup();
	void generateMissingMTL(fs::path &mtlFile);
	void flipOAndG();
	void debug();
	void setDirs(fs::path _objDir, fs::path _xlsDir, fs::path _mtlDir);
	void writeColorTable(fs::path _path);
	bool parseSceneBoundingBox(SceneHeader& sh, fs::path startPath, fs::path dstPath, bool first = true);

private:
	fs::path mPrivObjFolder;
	fs::path mPrivMtlFolder;
	fs::path mPrivXlsFolder;
	std::map<std::string, ooctools::MtlMap*> mPrivObjectMap;
	std::map<std::string, int> mPrivColorMultiverse;
	int writtenFileCount;
	int missMatches;
	ooctools::ColorTable ct;
};

} // end of namespace MTLGenerator

#endif /* MTLGENERATOR_H_ */
