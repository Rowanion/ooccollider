/**
 * @file	RawModelHandler.h
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 24.02.2009
 * @brief   Class declaration of RawModelHandler
 */

#ifndef RAWMODELHANDLER_H_
#define RAWMODELHANDLER_H_

#include <string>
#include <map>

#include "boost/filesystem.hpp"
#include <boost/filesystem/fstream.hpp>

#include "declarations.h"
#include "../../OOCTools/include/declarations.h"

namespace fs = boost::filesystem;
namespace oocformats {

/**
 * @class RawModelWriter
 * @brief Handlerclass for dealing with the fileformat for phase 1.
 */
class RawModelHandler {
public:
	RawModelHandler();
	virtual ~RawModelHandler();
	void testWrite(ooctools::MetaGroup *_grp);
	bool find_file(const fs::path & dir_path, const std::string & file_name, fs::path & path_found );
	char* readFileBytes(const char *name);
	void testRead();

	void writeHeader(ooctools::MetaGroup* _grp, fs::ofstream& _of);

	/**
	 * @param _model a pointer to the model, which will be saved as binary file
	 * @param _p a path (not a file) wher to save the model-files
	 */
	void writeModel(ooctools::Model* _model, fs::path _p);
	//std::map<std::string, OOCTools::VBO*> *readModel(fs::path _p);
	ooctools::VboManager* readModel(fs::path _p);

	/**
	 * @brief Reads a (binary)model from the pahe 1 format.
	 * @param _p Where to find the model
	 * @param _ct The colortable for the model.
	 * @return returns the instance of the vbomanager which holds the model after reading.
	 */
	ooctools::VboManager* readModel(fs::path _p, const ooctools::ColorTable& _ct);
	void testWriteArrayf(float* farray, int entries);
	void testReadArrayf(int entries);
	static ooctools::Vbo* readRawVbo(fs::path _path);
	static void writeRawVbo(ooctools::Vbo* _vbo, fs::path _path);

private:
	void testAndSetDir(fs::path _p);
	std::string unscrewOsgGroupName(const std::string& _gName);
	void removeSpecialCharsFromName(std::string& _origin);

	bool constructorCalled;

};

} // oocformats

#endif /* RAWMODELHANDLER_H_ */
